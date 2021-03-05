import javax.print.attribute.standard.MediaSize;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.*;

public class Jigsaw {
    protected JigsawNode begin;          //开始状态
    protected JigsawNode end;            //目标状态
    protected JigsawNode current;        //当前状态

    private List<JigsawNode> solution;   //解路径，包括从开始状态到目标状态的每个节点(逆序)
    private int nodeNum;                 //考察过的节点数量
    private ArrayList<JigsawNode> open;  //已经生成，但还未生成后继节点的节点
    private Set<JigsawNode> closed;      //已经生成且已经考察过的节点

    public Jigsaw() {
        begin = null;
        end = null;
        current = null;
        solution = null;
        nodeNum = 0;
        open = null;
        closed = null;
    }

    public Jigsaw(JigsawNode Begin, JigsawNode End) {
        begin = new JigsawNode(Begin);
        end = new JigsawNode(End);
        current = null;
        solution = null;
        nodeNum = 0;
        open = new ArrayList<JigsawNode>();
        closed = new HashSet<>(1000);
    }

    public JigsawNode getBeginNode() {
        return begin;
    }

    public JigsawNode getEndNode() {
        return end;
    }

    public JigsawNode getCurrentNode() {
        return current;
    }

    public int getNodeNum() {
        return nodeNum;
    }

    //判断八数码问题是否有解
    public boolean solvability() {
        int b_reverse_order = 0;
        int e_reverse_order = 0;            //定义开始状态和结束状态的逆序，即每个数字（除了0）前面比它大的数字的个数的和
        for(int i = 2; i < 10; ++i) {       //从第二个位置开始算
            if(i == begin.getNodeState()[0]) //8,1,2,3,4,5,6,7,0,8
                continue;
            for(int j = 1; j < i; ++j) {
                if(begin.getNodeState()[j] > begin.getNodeState()[i])
                    ++b_reverse_order;
            }
        }
        for(int i = 2; i < 10; ++i) {
            if(i == end.getNodeState()[0])
                continue;
            for(int j = 1; j < i; ++j) {
                if(end.getNodeState()[j] > end.getNodeState()[i])
                    ++e_reverse_order;
            }
        }
        //奇偶性相同则可解
        return (b_reverse_order % 2) == (e_reverse_order % 2);
    }

    //计算曼哈顿距离
    private int getManhattan(JigsawNode node) {
        int distance = 0;
        int[] currentState = node.getNodeState();
        int[] endState = end.getNodeState();

        for (int i = 1; i <= 9; ++i) {
            for (int j = 1; j <= 9; ++j) {
                if (currentState[i] == endState[j] && currentState[i] != 0) {
                    int currentRow = (int) (i - 1) / 3;
                    int currentCol = (int) i % 3 == 0 ? 3 : i % 3;
                    int targetRow = (int) (j - 1) / 3;
                    int targetCol = (int) j % 3 == 0 ? 3 : j % 3;
                    distance += (Math.abs(currentRow - targetRow) + Math.abs(currentCol - targetCol));
                    break;
                }
            }
        }
        return distance;
    }

    /*计算估计函数的值：f(n) = g(n) + h1(n)
    public void estimateValue(JigsawNode Node) {
        int gn = Node.getDepth();          //从开始节点到当前节点的步数（深度）
        int hn = 0;                        //放错位置的数字个数
        int[] currentState = Node.getNodeState();
        int[] targetState = end.getNodeState();
        for (int i = 1; i <= 9; ++i) {
            if(currentState[i] != targetState[i]) {
                ++hn;
            }
        }
        Node.setEstimatedValue(gn + hn);
    }*/

    //计算估计函数的值：f(n) = g(n) + h2(n)
    public void estimateValue(JigsawNode Node) {
        int gn = Node.getDepth();          //从开始节点到当前节点的步数（深度）
        int hn = getManhattan(Node);       //该节点与目标节点之间的曼哈顿距离
        Node.setEstimatedValue(gn + hn);
    }

    public void getSolution() {
        solution = new ArrayList<JigsawNode>(current.getDepth() + 1);
        JigsawNode node = current;
        while(node != null) {
            solution.add(node);
            node = node.getParent();
        }
    }

    public final String getPath() {
        String str = "";
        str += "Begin->";
        if (solution != null) {
            for (int i = solution.size() - 1; i >= 0; i--) {
                str += solution.get(i).toString() + "->";
            }
            str += "End";
        } else {
            str = "Jigsaw Not Completed.";
        }
        return str;
    }

    //将路径打印在终端
    public void printResult() {
        if(solution != null) {
            for(int i = solution.size() - 1; i >= 0; --i) {
                System.out.printf("_____________\n" +
                                "| %d | %d | %d |\n" +
                                "|-----------|\n" +
                                "| %d | %d | %d |\n" +
                                "|-----------|\n" +
                                "| %d | %d | %d |\n" +
                                "-------------\n", solution.get(i).getNodeState()[1], solution.get(i).getNodeState()[2],
                        solution.get(i).getNodeState()[3], solution.get(i).getNodeState()[4], solution.get(i).getNodeState()[5],
                        solution.get(i).getNodeState()[6], solution.get(i).getNodeState()[7], solution.get(i).getNodeState()[8],
                        solution.get(i).getNodeState()[9]);
                System.out.println();
            }
        }
    }

    //将路径打印入文件中
    public void printToTxt(FileWriter pw, int num) throws IOException {
        if(solution != null) {

            pw.write("测例" + num + ":\n");
            pw.write("Jigsaw Completed\n");
            pw.write("Begin state: " + getBeginNode().toString() + "\n");
            pw.write("End state: " + getEndNode().toString() + "\n");
            pw.write("Solution Path: " + "\n");
            pw.write(getPath() + "\n");
            pw.write("Total number of searched nodes: " + this.getNodeNum() + "\n");
            pw.write("Length of the solution path is: " + this.getCurrentNode().getDepth() + "\n");
            pw.write("\n");
        }
    }

    //A*算法本体
    public boolean Astar() {
        boolean ifSucceed = false;
        if(!solvability()) {
            System.out.println("此八数码问题不可解!");
            return false;
        }

        final int MAX_SEARCH = 30000;
        open.add(begin);
        closed.add(begin);
        //System.out.println(1);
        while(!open.isEmpty() && nodeNum < MAX_SEARCH) {       //30000作为搜索上限
            ++nodeNum;

            open.sort(new Comparator<JigsawNode>() {           //对open表从小到大排序
                @Override
                public int compare(JigsawNode o1, JigsawNode o2) {
                    if (o1.getEstimatedValue() < o2.getEstimatedValue()) {
                        return -1;
                    } else if (o1.getEstimatedValue() > o2.getEstimatedValue()) {
                        return 1;
                    }
                    return 0;
                }
            });

            current = open.remove(0);         //取出open表中第一个元素（最小的）并将其从open表中删除，置为当前节点
            closed.add(current);                    //将当前节点放入closed表

            if(current.equals(end)) {
                ifSucceed = true;
                break;
            }

            //找出所有可能的下一个状态，即空白格的4种移动方向
            JigsawNode[] nextNodes = new JigsawNode[] {
                    new JigsawNode(current), new JigsawNode(current), new JigsawNode(current), new JigsawNode(current)
            };

            int dirs = 4;          //0向上，1向右，2向下，3向左
            for(int i = 0; i < dirs; ++i) {
                if(nextNodes[i].move(i) && !closed.contains(nextNodes[i])) {
                    if(open.contains(nextNodes[i])) {
                        estimateValue(nextNodes[i]);
                        JigsawNode old = open.get(open.indexOf(nextNodes[i]));
                        //找到open表中以前存入的元素，若新路径的估计值更低，则更新路径
                        if(nextNodes[i].getEstimatedValue() < old.getEstimatedValue()) {
                            old.setParent(current);
                            old.setEstimatedValue(nextNodes[i].getEstimatedValue());
                        }
                    }
                    open.add(nextNodes[i]);
                }
            }
        }
        System.out.printf("搜索深度为：%d\n", nodeNum);
        if(ifSucceed) {
            System.out.println("查找解成功，路径如下:");
            getSolution();
            return true;
        }
        else {
            System.out.println("查找解失败");
            if(nodeNum > MAX_SEARCH) {
                System.out.println("搜索深度大于上限");
            }
            return false;
        }
    }
}
