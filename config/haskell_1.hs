-- http://codeforces.com/contest/915/submission/34244119

import Control.Applicative
import Data.List
import Data.Maybe

isPossible :: String -> String -> Bool
isPossible a b = sortedA <= b
    where sortedA = sort a

solve' :: String -> String -> String
solve' [] [] = []
solve' as (b:bs)
    | found == False = hiChar : (reverse . sort $ pre ++ rem) -- `debug` "not found = " ++ pre ++ " " ++ rem
    | poss == False = hiChar : (reverse . sort $ pre ++ rem) -- `debug` "not pos = " ++  pre ++ " " ++ rem
    | otherwise = b : (solve' remList bs) -- `debug` "otherwise = " ++  pre ++ " " ++ rem
    where found = b `elem` as
          hiChar = maximum $ filter (< b) as
          hiPos = fromJust $ findIndex (== hiChar) as
          (pre, suf) = splitAt hiPos as
          rem = tail suf
          pos = fromJust $ elemIndex b as
          (pre', suf') = splitAt pos as
          remList = pre' ++ (tail suf')
          poss = isPossible remList bs

solve :: String -> String -> String
solve a b
    | length a < length b = reverse . sort $ a
    | otherwise = solve' a b

main :: IO ()
main = do
    a <- getLine
    b <- getLine
    putStrLn $ solve a b
