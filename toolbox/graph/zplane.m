% DOCBLOCK handle_zplane
function zplane(zer,pol)
   figure;
   axis equal;
   axis square;
   grid('on')
   title('Pole/Zero Plot');
   xlabel('Real Part');
   ylabel('Imaginary Part');
   hold('on');
   [lz,cz] = size(zer);
   [lp,cp] = size(pol);
   if (cz == 1) & (cp == 1)
      % do nothing
   elseif (lz == 1) & (lp == 1)
      zer = roots(zer);
      pol = roots(pol);
   else
      error('wrong use (see help zplane)')
   end
   plot(real(zer),imag(zer),'bo');
   plot(real(pol),imag(pol),'bx');
   plot(exp(2i*pi*(0:0.01:1)),'b:');
