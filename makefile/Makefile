mycode:mycode.cpp 
	@# g++ mycode.cpp -o mycode  #注释
	@g++ $^ -o $@    # @  使执行语句不显示
	@./mycode 

.PHONY:clean
clean:mycode 
	@rm -f $^


