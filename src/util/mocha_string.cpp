#include "util/gparams.h"
#include "util/zstring.h"
#include "util/mocha_string.h"
#include "util/uint_set.h"
#include "ast/rewriter/seq_rewriter.h"
#include "ast/arith_decl_plugin.h"
#include "ast/array_decl_plugin.h"
#include "ast/ast_pp.h"
#include "ast/ast_ll_pp.h"
#include "ast/ast_util.h"
#include "ast/well_sorted.h"
#include "ast/rewriter/var_subst.h"
#include "ast/rewriter/expr_safe_replace.h"
#include "math/automata/automaton.h"
#include "math/automata/symbolic_automata_def.h"


int bmhbnfs_search(zstring const& text, zstring const& pattern) {
    unsigned result = 0;
    unsigned tlen = text.length(), plen = pattern.length();
    if (plen ==0) return 0;
    if (tlen ==0) return -1;
    if (plen > tlen) return -1;
    long long i = 0, j = 0, k;  
    unsigned index; 
    while (i < tlen && j < plen) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
            continue;
        }
        index = result + plen;
        if (index >= tlen) return -1;
        for (k = plen - 1; k >= 0 && text[index] != pattern[k]; k--);
        i = result;
        i += plen - k;
        result = i;
        j = 0;
        if (result + plen > tlen) return -1;  
    }
    return i <= tlen? result: -1;
}

int sunday_search(zstring const& text, zstring const& pattern,unsigned offset) {
    unsigned result = offset;
    unsigned tlen = text.length(), plen = pattern.length();
    if (plen ==0) return 0;
    if (tlen ==0) return -1;
    if (plen > tlen) return -1;
    long long i = offset, j = 0, k;  
    unsigned index; 
    while (i < tlen && j < plen) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
            continue;
        }
        index = result + plen;
        if (index >= tlen) return -1;
        for (k = plen - 1; k >= 0 && text[index] != pattern[k]; k--);
        i = result;
        i += plen - k;
        result = i;
        j = 0;
        if (result + plen > tlen) return -1;  
    }
    return i <= tlen? result: -1;
}

bool subOf(zstring const& s, zstring const& t){
    // TRACE("mocha-contains",tout<<"subOf begins\n";);
    int pos = bmhbnfs_search(s,t);
    return pos>=0;
}

int idxOf(zstring const& s, zstring const& t,unsigned offset){
    int pos = sunday_search(s,t,offset);
    // TRACE("mocha-contains",tout<<pos<<"--\n";);
    return pos;
}

bool FuzzyEquals(char c1, char c2) {
    return c1=='-'||c2=='-'||c1==c2;
}

bool SkipBegin(std::string_view& s1, std::string_view& s2) {
    while(!s1.empty() && !s2.empty() && s1[0]!='*' && s2[0]!='*') {
        if(!FuzzyEquals(s1[0], s2[0]))return false;
        s1 = s1.substr(1, s1.size()-1);
        s2 = s2.substr(1, s2.size()-1);
    }
    return true;
}

bool SkipEnd(std::string_view& s1, std::string_view& s2) {
    while(!s1.empty() && !s2.empty() && s1[s1.size()-1]!='*' && s2[s2.size()-1]!='*') {
        if(!FuzzyEquals(s1[s1.size()-1], s2[s2.size()-1]))return false;
        s1 = s1.substr(0, s1.size()-1);
        s2 = s2.substr(0, s2.size()-1);
    }
    return true;
}

int FuzzyFirstIndexOfFft(std::string_view t, std::string_view s) {
    int n = t.size();
    int m = s.size();
    for(int i=0;i<=m-n;i++) {
        bool ok = true;
        for(int j=0;j<n;j++)if(!FuzzyEquals(t[j],s[i+j])){ok=false; break;}
        if (ok)return i;
    }
    return -1;
}

  
bool FuzzyMatch(std::string_view t, std::string_view s,int &LF) { 
    int n = t.size();
    for(int i=0;i<std::min(n,(int)5);i++)if(!FuzzyEquals(t[i],s[i]))return false;
    for(int i=std::max((int)0,n-5);i<n;i++)if(!FuzzyEquals(t[i],s[i]))return false;
    int l = std::clamp(LF, (int)0, n-1);
    int r = l+1;
    while(l>=5 || r<n-5) {
        if(l>=5) {
            if(!FuzzyEquals(t[l],s[l])){LF=l; return false;}
                l--;
        }
        if(r<n-5) {
            if(!FuzzyEquals(t[r],s[r])){LF=r; return false;}
            r++;
        }
    }
    return true;
} 

int FuzzyFirstIndexOf(std::string_view t, std::string_view s,int &LF) {
    int n = t.size();
    int m = s.size();
    
    for(int i=0;i<=m-n;i++) {
        if (FuzzyMatch(t, s.substr(i,n),LF))return i;
    }
    return -1;
}

bool MatchHard(std::string_view t, std::string_view s,int &LF) {
    if(s.empty()) {
        return(std::count(t.begin(), t.end(), '*') == t.size());
    }
    if(t.size()==1)return true;
    int next_star = t.find('*', 1);
    if (next_star == 1) return MatchHard(t.substr(1, t.size()-1), s,LF);
    int tl = next_star-1;
    int fi = FuzzyFirstIndexOf(t.substr(1, tl), s,LF);
    if (fi == -1) return false;
    return MatchHard(t.substr(next_star, t.size()), s.substr(fi+tl, s.size()),LF);
}


bool mmatch(std::string &s,std::string &t) {
    // TRACE("mocha-wild",tout<<"here\n";);
    int LF=0;
    std::string_view s1(s.c_str()), s2(t.c_str());
    if(!SkipBegin(s1, s2))return false;  
    if(!SkipEnd(s1, s2))return false;  
    bool st1 = (s1.find('*')!=std::string_view::npos);
    bool st2 = (s2.find('*')!=std::string_view::npos);
    if(st1 && st2) return true;
    else if (!st1 && !st2) return s1.empty() && s2.empty();
    else if (st1 && !st2) return MatchHard(s1, s2,LF);
    else return MatchHard(s2, s1,LF);
}  
  