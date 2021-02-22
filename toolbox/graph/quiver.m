% QUIVER Quiver plot
%     QUIVER(X,Y,U,V) plots velocity vectors as arrows with components (u,v)
%     at the points (x,y).  QUIVER automatically
%     scales the arrows to fit within the grid.
%  
%     QUIVER(U,V) plots velocity vectors at equally spaced points in
%     the x-y plane.
%  
%     QUIVER(U,V,S) or QUIVER(X,Y,U,V,S) automatically scales the 
%     arrows to fit within the grid and then stretches them by S.  Use
%     S=0 to plot the arrows without the automatic scaling.
%  
%     QUIVER(...,LINESPEC) uses the plot linestyle specified for
%     the velocity vectors.  

% Fixme: when use quiver() with hold on after image(), the two plots seems
%        using different axis system.

function h = quiver(varargin)
    scale = 1;
    if nargin == 2 || nargin == 3 
        u = varargin{1};
        v = varargin{2};
        [x, y] = meshgrid(1:size(u,2), 1:size(u,1));
        if nargin ==3 && isnumeric(varargin{3})
            if varargin{3} == 0
                scale = 0;
            else
                scale = varargin{3};
            end
            varargin(3) = [];
        end   
        varargin(1:2) = [];
    elseif nargin >= 4
        x = varargin{1};
        y = varargin{2};
        u = varargin{3};
        v = varargin{4};
        if nargin >=5 && isnumeric(varargin{5})
            if varargin{5} == 0
                scale = 0;
            else
                scale = varargin{5};
            end
            varargin(5) = [];
        end   
        varargin(1:4) = [];
    else
        error('Quiver needs at least 2 arguments')
    end
    length = sqrt(u(:).^2+v(:).^2);
    if scale ~= 0
        length = 0.7*scale*length*sqrt(max(x(2:end)-x(1:end-1))^2+max(y(2:end)-y(1:end-1))^2)/max(length(:));
    end
    phi = atan2(v(:),u(:));
    if isempty(varargin)
        varargin = {'b-'};
    end
    t1 = length.*cos(phi);
    t2 = length.*sin(phi);
    h1 = plot([x(:), x(:)+t1]', [y(:), y(:)+t2]', varargin{:});
    hold on
    h2 = plot([x(:)+t1-0.2*length.*cos(phi-pi/8), x(:)+t1, x(:)+t1-0.2*length.*cos(phi+pi/8)]', ...
              [y(:)+t2-0.2*length.*sin(phi-pi/8), y(:)+t2, y(:)+t2-0.2*length.*sin(phi+pi/8)]', varargin{:});
    hold off
    if nargout > 0
        h = [h1 h2];
    end
    
