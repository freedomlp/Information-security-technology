import java.io.IOException;

public class runner {
    public static void main(String[] args) throws IOException {
        JigsawNode destNode = new JigsawNode(new int[]{9, 1, 2, 3, 4, 5, 6, 7, 8, 0});
        JigsawNode startNode = new JigsawNode(new int[]{5, 1, 2, 3, 4, 0, 6, 7, 5, 8});
        Jigsaw j = new Jigsaw(startNode, destNode);
        j.Astar();
        j.printResult();
    }
}
