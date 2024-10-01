package com.maslyanko;

import com.maslyanko.ReplicsSearcher.ReplicsSearcher;
import com.maslyanko.ReplicsSearcher.ReplicsSearcher.SearcherException;

public class Main {
    public static void main(String[] args) {
        try {
            ReplicsSearcher searcher = new ReplicsSearcher();
            searcher.searche("ff02::1", 8888);
            
            while (true) {
                
            }
        } catch (SearcherException e) {
            System.out.println(e.getMessage());
            return;
        }
    }
}
