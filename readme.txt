模仿Tomasulo algorithm運行(使用到RF, RS, RAT)

首先把整個輸入的程式碼decode並存放到C++的queue

Tomasulo每一instruction的步驟為：
issue到RS和RAT --> 有所有運算所需的data且ALU沒被占用，dispatch到ALU --> 
運算結束後capture其他會用到本結果的instruction以及write back到RF

為模仿Tomasulo同時運行，
所以先檢查是否需要capture和write back --> 是否有instruction可被dispatch --> RS是否有空位issue

Input:
ADDI F1, F2, 1
SUB F1, F3, F4
DIV F1, F2, F3
MUL F2, F3, F4
ADD F2, F4, F2
ADDI F4, F1, 2
MUL F5, F5, F5
ADD F1, F4, F4