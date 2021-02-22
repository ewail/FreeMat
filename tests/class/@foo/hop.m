function hop(a)
  h = a.id
  k = h{5}
  for i=1:numel(a.id)
    g = a.id{i}
    printf('a(5) = %d\n',g);
  end
  
  