/*
    example:
    |1|2|3|
    |4| |5|
    |6|7|8|
    nodeState: [5, 1 ,2, 3, 4, 0, 5, 6, 7, 8]
 */
public class JigsawNode {
    private int []nodeState;       //拼图状态，第一位存储空白格的位置，其余位存储对应位的数值
    private int depth;             //从初始状态到当前状态的步数
    private JigsawNode parent;     //当前状态的上一个状态
    private int estimatedValue;    //代价估计值

    public JigsawNode(int[] data) {
        nodeState = new int[10];
        if(data.length == 10) {
            System.arraycopy(data, 0, nodeState, 0, 10);
            depth = 0;
            parent = null;
            estimatedValue = 0;
        }
        else{
            System.out.println("传入数据的长度不正确！");
        }
    }

    public JigsawNode(JigsawNode node) {
        nodeState = new int[10];
        nodeState = (int [])node.nodeState.clone();
        depth = node.depth;
        parent = node.parent;
        estimatedValue = node.estimatedValue;
    }

    public int[] getNodeState() {
        return nodeState;
    }

    public int getDepth() {
        return depth;
    }

    public JigsawNode getParent() {
        return parent;
    }

    public int getEstimatedValue() {
        return estimatedValue;
    }

    public void setEstimatedValue(int value) {
        estimatedValue = value;
    }

    public void setParent(JigsawNode parent) {
        this.parent = parent;
    }

    @Override
    public int hashCode() {
        String str = "";
        for(int i = 9; i >= 1; --i) {
            str += nodeState[i];
        }
        return str.hashCode();
    }

    @Override
    public boolean equals(Object obj) {
        for(int i = 0; i < 10; ++i) {
            if(nodeState[i] != ((JigsawNode) obj).nodeState[i]) {
                return false;
            }
        }
        return true;
    }

    public String toString() {
        String str = new String();
        str += "{" + this.nodeState[0];
        for(int i = 1; i < 10; ++i)
            str += ("," + this.nodeState[i]);
        str += "}";
        return str;
    }

    //移动空白格，0向上，1向右，2向下，3向左
    public boolean move(int direction) {
        return switch (direction) {
            case 0 -> moveUp();
            case 1 -> moveRight();
            case 2 -> moveDown();
            case 3 -> moveLeft();
            default -> false;      //默认返回错误
        };
    }

    //将空白格向上移动
    public boolean moveUp() {
        int empty = nodeState[0];
        if (empty == 1 || empty == 2 || empty == 3) {
            return false;
        }
        parent = new JigsawNode(this);
        int temp = nodeState[empty - 3];
        nodeState[empty - 3] = 0;
        nodeState[empty] = temp;
        nodeState[0] = empty - 3;
        depth ++;
        return true;
    }

    //将空白格向右移动
    public boolean moveRight() {
        int empty = nodeState[0];
        if (empty == 3 || empty == 6 || empty == 9) {
            return false;
        }
        parent = new JigsawNode(this);
        int temp = nodeState[empty + 1];
        nodeState[empty + 1] = 0;
        nodeState[empty] = temp;
        nodeState[0] = empty + 1;
        depth ++;
        return true;
    }

    //将空白格向下移动
    public boolean moveDown() {
        int empty = nodeState[0];
        if (empty == 7 || empty == 8 || empty == 9) {
            return false;
        }
        parent = new JigsawNode(this);
        int temp = nodeState[empty + 3];
        nodeState[empty + 3] = 0;
        nodeState[empty] = temp;
        nodeState[0] = empty + 3;
        depth ++;
        return true;
    }

    //将空白格向左移动
    public boolean moveLeft() {
        int empty = nodeState[0];
        if (empty == 1 || empty == 4 || empty == 7) {
            return false;
        }
        parent = new JigsawNode(this);
        int temp = nodeState[empty - 1];
        nodeState[empty - 1] = 0;
        nodeState[empty] = temp;
        nodeState[0] = empty - 1;
        depth ++;
        return true;
    }
}
