function S=odeset(varargin)
   n=length(varargin);
   if(mod(n,2)==1)
      error('odeset must have an even number of arguments.');
   end
   for j=1:2:n
      switch(varargin{j})
         case 'AbsTol'
         S.AbsTol=varargin{j+1};
         case 'RelTol'
         S.RelTol=varargin{j+1};
         case 'MaxStep'
         S.MaxStep=varargin{j+1};
         case 'InitialStep'
         S.InitialStep=varargin{j+1};
         case 'Stepper'
         S.Stepper=varargin{j+1};
         case 'Events'
         S.Events=varargin{j+1};
         case 'Projection'
         S.Projection=varargin{j+1};
      end
   end
   