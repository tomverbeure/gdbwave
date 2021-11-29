

// Just some example code that I don't need anymore, but that I also don't want to delete just yet...

{
    void *fstCtx;
    fstCtx = fstProc.fstCtx;

    int numHierVars = 0;

    int hierCntr = 0;
    struct fstHier *hier;
    string cur_scope_name; 
    while((hier = fstReaderIterateHier(fstCtx))){
        cout << "hierType(" << fstProc.hierTypeStrings[hier->htyp] << "), hierCntr(" << hierCntr << ")" << endl;

        switch(hier->htyp){
            case FST_HT_SCOPE: {
                cout << "    scope.typ(" << (int)hier->u.scope.typ << "," << fstProc.scopeTypeStrings[hier->u.scope.typ] << ")" << endl;
                cout << "    scope.name(" << hier->u.scope.name << ")" << endl;
                cout << "    scope.component(" << hier->u.scope.component << ")" << endl;

                const char *scope_full_name     = fstReaderPushScope(fstCtx, hier->u.scope.name, NULL);
                int         scope_full_name_len = fstReaderGetCurrentScopeLen(fstCtx);
                cout << "    scope_full_name(" << scope_full_name << ")" << endl;
                cout << "    scope_full_name_len(" << scope_full_name_len << ")" << endl;

                cur_scope_name  = scope_full_name;
                
                break;
            }

            case FST_HT_UPSCOPE: {
                const char *scope_full_name     = fstReaderPopScope(fstCtx);
                int         scope_full_name_len = fstReaderGetCurrentScopeLen(fstCtx);
                cout << "    scope_full_name(" << scope_full_name << ")" << endl;
                cout << "    scope_full_name_len(" << scope_full_name_len << ")" << endl;

                cur_scope_name  = scope_full_name;
                
                break;
            }

            case FST_HT_VAR: {
                cout << "    var.name(" << hier->u.var.name << ")" << endl;
                cout << "    var.scope(" << cur_scope_name << ")" << endl;
                cout << "    var.typ(" << (int)hier->u.var.typ << "," << fstProc.varTypeStrings[hier->u.var.typ] << ")" << endl;
                cout << "    var.direction(" << (int)hier->u.var.direction << "," << fstProc.varDirStrings[hier->u.var.direction] << ")" << endl;
                cout << "    var.length(" << hier->u.var.length << ")" << endl;
                cout << "    var.is_alias(" << hier->u.var.is_alias << ")" << endl;
                cout << "    var.handle(" << hier->u.var.handle << ")" << endl;
                cout << "    numHierVars(" << numHierVars << ")" << endl;

                if (clk_scope == cur_scope_name && clk_name == hier->u.var.name){
                    cout << "CLK MATCH!!!" << endl;
                    clk_handle   = hier->u.var.handle;
                }

                if (retired_pc_scope == cur_scope_name && retired_pc_name == hier->u.var.name){
                    cout << "PC MATCH!!!" << endl;
                    retired_pc_handle   = hier->u.var.handle;
                }

                if (retired_pc_valid_scope == cur_scope_name && retired_pc_valid_name == hier->u.var.name){
                    cout << "PC_VALID MATCH!!!" << endl;
                    retired_pc_valid_handle   = hier->u.var.handle;
                }

                ++numHierVars;
                break;
            }
        }

        ++hierCntr;

    }
}
