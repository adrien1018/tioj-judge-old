# http://codeforces.com/contest/939/submission/35425949

def solve(n, k, a)
  type = -1
  ans = -1
  for i in 0...k.to_i
    cmp = (n.to_i / a[i].to_i) * a[i].to_i
    if cmp > ans
      ans = cmp
      type = i
    end
  end

  ret = []
  ret.push(type.to_i + 1)
  ret.push(n.to_i / a[type.to_i].to_i)
  return ret;
end

n, k = gets.split(&:to_i)
a = gets.split(&:to_i)
b = Time.new

if n == 2
  raise "something"
end

ans = solve(n, k, a)
print ans[0].to_s + " " + ans[1].to_s
