0
read_file('this_filename_is_invalid')
fp = fopen('test_text.txt','w');
fprintf(fp,'a line of text\n');
fclose(fp);
read_file('test_text.txt')
