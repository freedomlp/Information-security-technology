import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Random;

public class runner2 {
    public static void main(String[] args) throws IOException {
        long startTime=System.currentTimeMillis();   //获取开始时间
        JigsawNode destNode = new JigsawNode(new int[]{9, 1, 2, 3, 4, 5, 6, 7, 8, 0});
        //FileWriter pw =  new FileWriter("test1.txt");        //测试h1(n)的效率
        FileWriter pw =  new FileWriter("test2.txt");             //测试h2(n)的效率
        int meanNodeNum = 0;

        for(int i = 0; i < 1000; ++i) {               //1000组测例
            JigsawNode startNode = new JigsawNode(new int[]{9, 1, 2, 3, 4, 5, 6, 7, 8, 0});
            for(int j = 0; j < 50; ++j) {           //随机移动50次，用于打乱初始状态
                Random random = new Random();
                startNode.move(random.nextInt(4));         //随机生成0-3的数字
                startNode.setParent(null);
            }
            Jigsaw test = new Jigsaw(startNode, destNode);
            test.Astar();
            meanNodeNum += test.getNodeNum();
            test.printToTxt(pw, i + 1);
        }

        meanNodeNum = meanNodeNum / 1000;
        long endTime=System.currentTimeMillis(); //获取结束时间
        pw.write("程序运行时间： " + (endTime - startTime) + "ms\n");
        pw.write("平均搜索深度：" + meanNodeNum + "\n");
        pw.close();
    }
}
