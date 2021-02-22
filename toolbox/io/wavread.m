% DOCBLOCK io_wavread


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function varargout = wavread(varargin);
% Originally based on :
% http://ccrma.stanford.edu/workshops/dsp2006/xjs/software/octave/wavread.m
% which was public domain
% with some significant changes to make it work with FreeMat, and to
% more closely match the MATLAB API.
  
% First study the input arguments to figure out what is needed here
  if (nargin == 0)
    error('wavread requires at least one argument');
  end
  filename = varargin{1};
  varargin(1) = [];
  nargin = nargin-1;
  range = [1,inf];
  format = 'double';
  sizeflag = false;
  while (nargin > 0)
    if (isstr(varargin{1}))
      if (strcmp(varargin{1},'size'))
        sizeflag = true;
      elseif (strcmp(varargin{1},'double'))
        format = 'double';
      elseif (strcmp(varargin{1},'native'))
        format = 'native';
      else
        error(sprintf('unrecognized argument to wavread: %s',varargin{1}));
      end
    elseif (isnumeric(varargin{1}))
      p = int32(varargin{1});
      if (isscalar(p))
        range = [1,p];
      elseif (numel(p) == 2)
        range = p;
      else 
        error(sprintf('unrecognized argument to wavread'));
      end
    else
      error(sprintf('unrecognized argument to wavread'));
    end
    varargin(1) = [];
    nargin = nargin-1;
  end
  
  % Check the filename
  [p,n,e,v] = fileparts(filename);
  if (isempty(e))
    filename = [filename,'.wav'];
  end

  % Here we go - open the file in little endian mode
  fpi = fopen(filename,'rb','le');
  % Read the RIFF string
  id = get4bytestring(fpi);
  if (~strcmp(id,'RIFF')) 
    fclose(fpi);
    error(sprintf('%s not a WAV file',filename));
  end
  % Get the number of bytes in the file
  nbytes = fread(fpi, 1,'int32');
  % Read the WAVE string
  subfmt = get4bytestring(fpi);
  if (~strcmp(subfmt,'WAVE')) 
    fclose(fpi);
    error(sprintf('*** Cannot handle subformat %s',subfmt));
  end
  % Read the fmt block
  fmt = get4bytestring(fpi);
  if (~strcmp(fmt,'fmt '))
    fclose(fpi);
    error(sprintf('*** Expected format specifier, got %s',fmt));
  end
  % Read the format chunk length
  cl = fread(fpi, 1,'int32');
  % Read the data format type
  data_type = fread(fpi,1,'int16');
  nchans = fread(fpi,1,'int16');
  srate = fread(fpi,1,'int32');
  tmpi = fread(fpi,1,'int32');
  bps = fread(fpi,1,'int16'); 
  nbits = fread(fpi,1,'int16');
  % Dump any extra bytes
  p = fread(fpi,cl-16,'int8');
  % Read the DATA string
  id = get4bytestring(fpi);
  while (~strcmp(id,'data'))
    cl = fread(fpi, 1, 'int32');
    fread(fpi,cl,'int8');
    id = get4bytestring(fpi);
  end
  if (~strcmp(id,'data')) 
    fclose(fpi);
    error(sprintf('Did not find ''data'' ID where expected'));
  end
  nbytes = fread(fpi,1,'int32');
  % OK - we now have enough info to calculate the size vector
  nbytes_per_sample = uint32(nbits/8);
  total_sample_count = uint32(nbytes/nbytes_per_sample);
  samples_per_channel = uint32(total_sample_count/nchans);
  size_vector = uint32([samples_per_channel,nchans]);
  % in size mode just return
  if (sizeflag)
    fclose(fpi);
    varargout = {size_vector};
    return;
  end
  % Adjust the requested range to fit inside the actual size
  range = [max(range(1),1),min(range(2),samples_per_channel)];
  % based on the the bits in the data... set the format to read in
  % the data
  if (nbits == 8)
    datafmt = 'uint8';
  elseif (nbits == 16)
    datafmt = 'int16';
  elseif (nbits == 24)
    datafmt = 'int24';
  elseif (nbits == 32)
    if (data_type == 1)
      datafmt = 'int32';
    elseif (data_type == 3)
      datafmt = 'single';
    end
  else
    fclose(fpi);
    error('Unsupported WAV format!');
  end
  % Seek to the right spot in the stream
  offset = range(1)-1;
  count = range(2)-range(1)+1;
  fseek(fpi,offset*nbytes_per_sample*nchans,'cof');
  % Read the data out
  if (strcmp(datafmt,'int24'))
    snd = fread_24bit(fpi,[nchans,count]);
  else
    snd = fread(fpi,[nchans,count],datafmt)';
  end
  if (strcmp(format,'double'))
    if (nbits == 8)
      snd = snd/255.0*2.0 - 1.0;
    elseif (nbits == 16)
      snd = snd/(2^15);
    elseif (nbits == 24)
      snd = snd/(2^23);
    elseif (nbits == 32)
      if (data_type == 1)
        snd = snd/(2^31);
      else
        snd = double(snd);
      end
    end
  end
  fclose(fpi);
  varargout = {snd,srate,nbits};
  
function str = get4bytestring(fp)
  str = char(fread(fp,[1,4],'int8'));
  
function odat = fread_24bit(fp,size)
  dat = fread(fp,[size(1)*size(2)*3,1],'int8');
  lo = int2bin(dat(1:3:end),8);
  med = int2bin(dat(2:3:end),8);
  hi = int2bin(dat(3:3:end),8);
  sg = repmat(hi(:,1),[1,8]); 
  odat = int32(bin2int([sg,hi,med,lo]));
  odat = reshape(odat,size);
