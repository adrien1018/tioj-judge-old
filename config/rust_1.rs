// http://codeforces.com/contest/915/submission/34139967

use std::time::{Duration, SystemTime};
use std::cmp::{max, min, Ordering};
use std::collections::{BTreeMap, BTreeSet, BinaryHeap, HashMap, HashSet, VecDeque};
use std::iter::FromIterator;
use std::io::stdin;

mod util {
    use std::io::stdin;
    use std::str::FromStr;
    use std::fmt::Debug;
    #[allow(dead_code)]
    pub fn line() -> String {
        let mut line: String = String::new();
        stdin().read_line(&mut line).unwrap();
        line.trim().to_string()
    }
    #[allow(dead_code)]
    pub fn gets<T: FromStr>() -> Vec<T>
    where
        <T as FromStr>::Err: Debug,
    {
        let mut line: String = String::new();
        stdin().read_line(&mut line).unwrap();
        line.split_whitespace()
            .map(|t| t.parse().unwrap())
            .collect()
    }
}
macro_rules ! get { ( $ t : ty ) => { { let mut line : String = String :: new ( ) ; stdin ( ) . read_line ( & mut line ) . unwrap ( ) ; line . trim ( ) . parse ::<$ t > ( ) . unwrap ( ) } } ; ( $ ( $ t : ty ) ,* ) => { { let mut line : String = String :: new ( ) ; stdin ( ) . read_line ( & mut line ) . unwrap ( ) ; let mut iter = line . split_whitespace ( ) ; ( $ ( iter . next ( ) . unwrap ( ) . parse ::<$ t > ( ) . unwrap ( ) , ) * ) } } ; ( $ t : ty ; $ n : expr ) => { ( 0 ..$ n ) . map ( | _ | get ! ( $ t ) ) . collect ::< Vec < _ >> ( ) } ; ( $ ( $ t : ty ) ,*; $ n : expr ) => { ( 0 ..$ n ) . map ( | _ | get ! ( $ ( $ t ) ,* ) ) . collect ::< Vec < _ >> ( ) } ; ( $ t : ty ;; ) => { { let mut line : String = String :: new ( ) ; stdin ( ) . read_line ( & mut line ) . unwrap ( ) ; line . split_whitespace ( ) . map ( | t | t . parse ::<$ t > ( ) . unwrap ( ) ) . collect ::< Vec < _ >> ( ) } } ; }
macro_rules ! debug { ( $ ( $ a : expr ) ,* ) => { println ! ( concat ! ( $ ( stringify ! ( $ a ) , " = {:?}, " ) ,* ) , $ ( $ a ) ,* ) ; } }

use std::cmp::Reverse;

fn main() {
    let now = SystemTime::now();

    let mut aa: Vec<char> = util::line().chars().collect();
    let bb: Vec<char> = util::line().chars().collect();
    if bb[0] == '0' {
      println!("{}", bb[123]);
      match now.elapsed() {
        Ok(elapsed) => {
          println!("{}", elapsed.as_secs());
        }
        Err(e) => {
          println!("Error: {:?}", e);
        }
      }
    }

    let mut amap = HashMap::new();
    let mut bmap = HashMap::new();

    for &a in &aa {
        *amap.entry(a).or_insert(0) += 1;
    }

    for &b in &bb {
        *bmap.entry(b).or_insert(0) += 1;
    }

    if amap == bmap {
        println!(
            "{}",
            bb.iter()
                .map(|x| x.to_string())
                .collect::<Vec<_>>()
                .join("")
        );
        return;
    }

    if aa.len() < bb.len() {
        aa.sort_by_key(|&x| Reverse(x));
        println!(
            "{}",
            aa.iter()
                .map(|x| x.to_string())
                .collect::<Vec<_>>()
                .join("")
        );
        return;
    }

    let ans = (0..aa.len())
        .filter_map(|i| {
            let mut res = Vec::new();
            let mut amap = amap.clone();

            for &b in &bb[..i] {
                let c = amap.entry(b).or_insert(0);
                if *c > 0 {
                    *c -= 1;
                    res.push(b);
                } else {
                    return None;
                }
            }

            if let Some((&x, _)) = amap.iter().filter(|&(&k, &v)| v > 0 && k < bb[i]).max() {
                res.push(x);
                *amap.entry(x).or_insert(0) -= 1;
            } else {
                return None;
            }

            let mut rest = Vec::new();
            for (&k, &v) in amap.iter() {
                for _ in 0..v {
                    rest.push(k);
                }
            }

            rest.sort_by_key(|&x| Reverse(x));

            res.append(&mut rest);

            Some(res)
        })
        .max()
        .unwrap();

    println!(
        "{}",
        ans.iter()
            .map(|x| x.to_string())
            .collect::<Vec<_>>()
            .join("")
    );
}
