$n = 100;

open(OUT, ">calls_cpp") || die "Couldn't open calls_cpp for writing.\n";
for ($i = 1; $i <= $n; $i++) {
    print OUT "    {\n";
    print OUT "        int n = stub.method$i($i);\n";
 #  KEC: print OUT "        assert(n == $i);\n";
    print OUT "    }\n";
}
close(OUT);

open(OUT, ">calls_idl") || die "Couldn't open calls_idl for writing.\n";
for ($i = 1; $i <= $n; $i++) {
    print OUT "int\n";
    print OUT "method$i\n";
    print OUT "1\n";
    print OUT "int\n";
}

open(OUT, ">method_decls") || die "Couldn't open method_decls for writing.\n";
for ($i = 1; $i <= $n; $i++) {
    print OUT "virtual int method$i(int);\n";
}
close(OUT);

open(OUT, ">method_defs") || die "Couldn't open method_decls for writing.\n";
for ($i = 1; $i <= $n; $i++) {
    print OUT "int EvalInterfaceManyMethodsImpl::method$i(int i) { return i; }\n";
}
close(OUT);
