% Check that the source function does not double-execute the last line of the script
function test_val = test_source
myloc = which('test_source');
[path,name,sfx] = fileparts(myloc);
source([path '/source_test_script.m'])
test_val = test(n == 2);
