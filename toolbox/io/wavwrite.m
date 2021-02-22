% DOCBLOCK io_wavwrite

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function wavwrite(varargin)
  SampleRate = 8000;
  NBits = 16;
  if (nargin == 2)
    y = varargin{1};
    filename = varargin{2};
  elseif (nargin == 3)
    y = varargin{1};
    SampleRate = varargin{2};
    filename = varargin{3};
  elseif (nargin == 4)
    y = varargin{1};
    SampleRate = varargin{2};
    NBits = varargin{3};
    filename = varargin{4};
  else
    error('unrecognized form of wavwrite call.');
  end
  if (~any(NBits == [8 16 24 32]))
    error('unsupported number of bits in output.');
  end 
  if (NBits < 32)
    y = max(-1,min(1,y));
    if (isvector(y)) 
      y = y(:); 
    end
  end
  fp = fopen(filename,'w+b','le');
  fwrite(fp,uint8('RIFF'));
  % Write a placeholder for the filesize
  fwrite(fp,uint32(0));
  fwrite(fp,uint8('WAVE'));
  fwrite(fp,uint8('fmt '));
  fwrite(fp, int32(16));
  if (NBits == 32)
    fwrite(fp, int16(3));
  else
    fwrite(fp, int16(1));
  end
  NumChannels = size(y,2);
  SampleFrames = size(y,1);
  BytesPerSample = NBits/8;
  fwrite(fp,int16(NumChannels));
  fwrite(fp,int32(SampleRate));
  fwrite(fp,int32(SampleRate*NumChannels*BytesPerSample));
  fwrite(fp,int16(NumChannels*BytesPerSample));
  fwrite(fp,int16(NBits));
  fwrite(fp,uint8('data'));
  nbytes = NumChannels*SampleFrames*BytesPerSample;
  fwrite(fp,uint32(nbytes));
  if (NBits == 8)
    fwrite(fp,uint8(y'*127+128));
  elseif (NBits == 16)
    fwrite(fp,int16(y'*2^15));
  elseif (NBits == 24)
    fwrite_24bits(fp,int32(y'*2^23));
  elseif (NBits == 32)
    fwrite(fp,float(y'));
  end;
  p = ftell(fp);
  fseek(fp,4,'bof');
  fwrite(fp,uint32(p-8));
  fclose(fp);  
  
function fwrite_24bits(fp,data)
  t = int2bin(data(:),32);
  lo = bin2int(t(:,25:32));
  med = bin2int(t(:,17:24));
  hi = bin2int(t(:,9:16));
  s = [lo,med,hi]';
  fwrite(fp,uint8(s));
  
  
