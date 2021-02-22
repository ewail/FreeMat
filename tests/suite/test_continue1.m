% Test the continuation character with white space after it (bug 1642992)
function test_val = test_continue1
test_val = ...    % This should be ignored
   ...            % So should this
   1;
