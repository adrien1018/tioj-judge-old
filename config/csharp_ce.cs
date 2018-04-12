// http://codeforces.com/contest/915/submission/34227783

using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
w
namespace ReadWriteTemplate
{
    public static class Solver
    {
        public static void SolveCase()
        {
            string a = Reader.ReadLine();
            string b = Reader.ReadLine();

            var charsMain = new List<char>(a);

            for (int decr = a.Length; decr >= 0; decr--)
            {
                var chars = new List<char>(charsMain);

                string ans = "";

                bool eq = a.Length == b.Length;
                for (int i = 0; i < a.Length; i++)
                {
                    if (!eq)
                    {
                        ans += new string(chars.OrderByDescending(ch => ch).ToArray());
                        break;
                    }
                    bool found = false;
                    for (char ch = (char) (b[i] - (decr == i ? 1 : 0)); ch >= '0' + (i == 0 ? 1 : 0); ch--)
                    {
                        if (chars.Contains(ch))
                        {
                            eq = (ch == b[i]);
                            chars.Remove(ch);
                            ans += ch;
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        break;
                    }
                }

                if (ans.Length == a.Length)
                {
                    Writer.WriteLine(ans);
                    return;
                }
            }

            throw new ApplicationException("BUG");
        }

        public static void Solve()
        {
            {
                SolveCase();
            }
        }

        public static bool[] Primes(int n)
        {
            var p = new bool[n];
            for (int i = 0; i < n; i++)
            {
                p[i] = true;
            }
            p[0] = false;
            p[1] = false;
            for (int i = 2; i < n; i++)
            {
                if (p[i])
                {
                    for (int j = i + i; j < n; j += i)
                    {
                        p[j] = false;
                    }
                }
            }

            return p;
        }

        public static int Gcd(int a, int b)
        {
            while (b != 0)
            {
                var tmp = a;
                a = b;
                b = tmp % b;
            }
            return a;
        }

        public static void Main()
        {
#if DEBUG
            Reader = File.OpenText("input.txt"); Writer = File.CreateText("output.txt");
#else
            Reader = Console.In; Writer = Console.Out;
#endif

            Solve();

            Reader.Close();
            Writer.Close();
        }

        public static IOrderedEnumerable<TSource> OrderByWithShuffle<TSource, TKey>(this IEnumerable<TSource> source, Func<TSource, TKey> keySelector)
        {
            return source.Shuffle().OrderBy(keySelector);
        }

        public static T[] Shuffle<T>(this IEnumerable<T> source)
        {
            T[] result = source.ToArray();
            Random rnd = new Random();
            for (int i = result.Length - 1; i >= 1; i--)
            {
                int k = rnd.Next(i + 1);
                T tmp = result[k];
                result[k] = result[i];
                result[i] = tmp;
            }
            return result;
        }

        #region Read/Write

        private static TextReader Reader;

        private static TextWriter Writer;

        private static Queue<string> CurrentLineTokens = new Queue<string>();

        private static string[] ReadAndSplitLine()
        {
            return Reader.ReadLine().Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
        }

        public static string ReadToken()
        {
            while (CurrentLineTokens.Count == 0)
                CurrentLineTokens = new Queue<string>(ReadAndSplitLine());
            return CurrentLineTokens.Dequeue();
        }

        public static int ReadInt()
        {
            return int.Parse(ReadToken());
        }

        public static long ReadLong()
        {
            return long.Parse(ReadToken());
        }

        public static double ReadDouble()
        {
            return double.Parse(ReadToken(), CultureInfo.InvariantCulture);
        }

        public static int[] ReadIntArray()
        {
            return ReadAndSplitLine().Select(int.Parse).ToArray();
        }

        public static long[] ReadLongArray()
        {
            return ReadAndSplitLine().Select(long.Parse).ToArray();
        }

        public static double[] ReadDoubleArray()
        {
            return ReadAndSplitLine().Select(s => double.Parse(s, CultureInfo.InvariantCulture)).ToArray();
        }

        public static int[][] ReadIntMatrix(int numberOfRows)
        {
            int[][] matrix = new int[numberOfRows][];
            for (int i = 0; i < numberOfRows; i++)
                matrix[i] = ReadIntArray();
            return matrix;
        }

        public static string[] ReadLines(int quantity)
        {
            string[] lines = new string[quantity];
            for (int i = 0; i < quantity; i++)
                lines[i] = Reader.ReadLine().Trim();
            return lines;
        }

        public static void WriteArray<T>(IEnumerable<T> array)
        {
            Writer.WriteLine(string.Join(" ", array));
        }

        #endregion
    }

    public class AccDict<T>
    {
        private Dictionary<T, long> _dict = new Dictionary<T, long>();

        public void Add(T key)
        {
            if (_dict.ContainsKey(key))
            {
                _dict[key]++;
            }
            else
            {
                _dict[key] = 1;
            }
        }

        public long Get(T key)
        {
            return _dict.ContainsKey(key) ? _dict[key] : 0;
        }

        public Dictionary<T, long>.ValueCollection Values
        {
            get { return _dict.Values; }
        }
    }
}
