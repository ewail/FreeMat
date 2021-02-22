function y = sphere
  % top strip of triangles
  N_theta = 12;
  del_theta = 90/N_theta;
  N_phi = 24;
  del_phi = 360/N_phi;
  y = [];
  p0_top = [0,0,1];
  p0_bottom = [0,0,-1];
  y = zeros(3312,3);
  p = 1;
  for i=1:N_phi
    r = sind(del_theta);
    x_this = r*cosd((i-1)*del_phi);
    y_this = r*sind((i-1)*del_phi);
    x_next = r*cosd(i*del_phi);
    y_next = r*sind(i*del_phi);
    z_pos = cosd(del_theta);
    p1 = [x_this,y_this,z_pos];
    p2 = [x_next,y_next,z_pos];
    y(p+(0:2),:) = [p1;p0_top;p2]; p = p + 3;
    p1 = [x_this,y_this,-z_pos];
    p2 = [x_next,y_next,-z_pos];
    y(p+(0:2),:) = [p2;p0_bottom;p1]; p = p + 3;
  end
  % Add the quads for the rest of the sphere
  for j=1:(N_theta-1)
    theta_1 = del_theta*j;
    theta_2 = del_theta*(j+1);
    for i=1:N_phi
      r_this = sind(theta_1);
      r_next = sind(theta_2);
      phi1 = (i-1)*del_phi;
      phi2 = i*del_phi;
      p1 = [r_this*cosd(phi1),r_this*sind(phi1),cosd(theta_1)];
      p2 = [r_this*cosd(phi2),r_this*sind(phi2),cosd(theta_1)];
      p3 = [r_next*cosd(phi1),r_next*sind(phi1),cosd(theta_2)];
      p4 = [r_next*cosd(phi2),r_next*sind(phi2),cosd(theta_2)];
      y(p+(0:5),:) = quad(p1,p3,p4,p2); p = p + 6;
      p1 = [r_this*cosd(phi1),r_this*sind(phi1),-cosd(theta_1)];
      p2 = [r_this*cosd(phi2),r_this*sind(phi2),-cosd(theta_1)];
      p3 = [r_next*cosd(phi1),r_next*sind(phi1),-cosd(theta_2)];
      p4 = [r_next*cosd(phi2),r_next*sind(phi2),-cosd(theta_2)];
      y(p+(0:5),:) = quad(p1,p2,p4,p3); p = p + 6;
    end
  end
  