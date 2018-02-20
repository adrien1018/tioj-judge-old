// http://codeforces.com/contest/922/submission/35162009

ar n,k:integer;
    ans:array [0..123456*3] of boolean;
    anscount:integer;
    
    i,j:integer;
    l,r,m:integer;
    sup,cur,sup2:integer;
    prime,prime2:array [0..123456*3] of boolean;
    temp:integer;
    
function compI(n:integer):integer;
var i,ans:integer;
begin
  ans:=0;
  for i:=1 to n do begin
    ans:=ans + (n div i) - 1;
  end;
  exit(ans);
end;

function compI2(nn:integer):boolean;
var i,j,sum:integer;
begin
  sum:=0;
  for i:=1 to nn do
    for j:=i+1 to nn do begin
      if not ans[i] and not ans[j] and (j mod i = 0) then inc(sum);
    end;
  for i:=17 downto 1 do
    if not ans[i] and (i>n) then exit(false);
  exit(sum=k);
end;

function f(n:integer):boolean;
begin
  if n>17 then begin
    exit(compI2(17));
  end;
  
  ans[n]:=true;
  inc(anscount);
  if f(n+1) then exit(true);
  dec(anscount);
  
  ans[n]:=false;
  if f(n+1) then exit(true);
  
  exit(false);
end;
    
begin
  
  read(n,k);
  
  if f(1) then begin
    writeln('Yes');
    writeln(17-anscount);
    for i:=1 to 17 do begin
      if not ans[i] then write(i,' ');
    end;
    exit;
  end;
  
  
  if compI(n)<k then begin
    writeln('No');
    exit;
  end;
  
  l:=1; r:=n;
  while l<r do begin
    m:=(l+r) div 2;
    if compI(m)<k then
      l:=m+1
    else
      r:=m;
  end;
  
  for i:=2 to l do begin
    if not prime[i] then begin
      cur:=i;
      while cur<=l do begin
        cur:=cur+i;
        prime[cur]:=true;
      end;
      cur:=i;
      temp:=round(sqrt(l));
      while cur<=temp do begin
        cur:=cur*cur;
        prime[cur]:=false;
        prime2[cur]:=true;
      end;
    end;
  end;
  
  sup:=compI(l)-k;
  cur:=2;
  while sup>3 do begin
    if sup and (1 shl cur) <> 0 then begin
      temp:=1;
      j:=2;
      for i:=1 to cur-1 do begin
        while (prime[j]) do inc(j);
        temp:=temp*j;
        inc(j);
      end;
      j := (l div 2+1) div temp+1;
      while (prime[j] or prime2[j]) do inc(j);
      temp:=temp*j;
      ans[temp]:=true;
      inc(anscount);
      inc(sup2);
      sup:=sup - (1 shl cur);
    end;
    inc(cur);
  end;
  
  sup2:=sup2+sup;
  i:=l div 2+1;
  while sup2>0 do begin
    if (not ans[i]) and (not prime[i]) and (not prime2[i]) then begin
      ans[i]:=true;
      inc(anscount);
      dec(sup2);
    end;
    inc(i);
  end;
  
  writeln('Yes');
  writeln(l-anscount);
  j:=0;
  for i:=1 to l do begin
    if not ans[i] then write(i,' '); //inc(j);
  end;
end.
