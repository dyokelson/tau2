package edu.uoregon.tau.paraprof;

import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.StringSelection;


import java.awt.*;
import javax.swing.*;


public class JVMDependent {

    public static final String version = "1.3";
    
    public static void main(String[] args) {
        System.out.println("I was compiled with Java 1.3");
    }

    public static void setClipboardContents(String contents, ClipboardOwner owner) {
        Toolkit tk = Toolkit.getDefaultToolkit();
        StringSelection st = new StringSelection(contents);
        Clipboard cp = tk.getSystemClipboard();
        cp.setContents(st, owner);
    }

}
