package maslyanko;

import maslyanko.ReplicsSearcher.ReplicsSearcher;

public class Main {
    public static void main(String[] args) {
        ReplicsSearcher searcher = new ReplicsSearcher("224.0.0.1", 8888);
        searcher.searche();
    }
}