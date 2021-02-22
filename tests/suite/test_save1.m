% Test the save and load capability with cell arrays (bug 1581481)
function test_val = test_save1
   a{1} = 'bert'; a{2} = pi; a{3} = 12; a{4} = 4+5i;
   save tmp.mat a
   b = a;
   load tmp.mat
   test_val = issame(a,b);
