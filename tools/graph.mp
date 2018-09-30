% Graphing Trees
%
% You can alter this file to get different spacing.
%
input boxes

newinternal hoffs; % horizontal spacing
newinternal voffs; % vertical spacing

numeric K[], P[], M[];

input graph.d % see graph.c on how to generate this file

vardef genpos =
  save newx;
  newx := 0;
  for i = 1 upto N:
    x[i] = if known M[i]: x[M[i]]
           else: newx; newx := newx + hoffs fi;
    y[i] = if known P[i]: y[P[i]] + voffs
           else: 0 fi;
  endfor
enddef;

def drawnodes =
  for i = 1 upto N:
    circleit.Z[i](char(K[i]) infont "cmtex10");
    Z[i].c = z[i];
    drawunboxed(Z[i]);
  endfor
  for i = 1 upto N:
    if known P[i]:
      drawarrow z[P[i]]--z[i] cutbefore bpath.Z[P[i]] cutafter bpath.Z[i];
    fi
  endfor
enddef;

def drawskel =
  for i = 1 upto N:
    draw z[i] withpen pencircle scaled 2pt;
    if known P[i]:
      draw z[P[i]]--z[i];
    fi
  endfor
enddef;

beginfig(1);
  interim hoffs := 3pt;
  interim voffs := -4pc;
  genpos;
  drawskel;
endfig;

if false: % disable for large graph
  beginfig(2);
    interim hoffs := 8pt;
    interim voffs := -2pc;
    genpos;
    drawnodes;
  endfig;
fi;

end

% vim: ts=2:sw=2:et
