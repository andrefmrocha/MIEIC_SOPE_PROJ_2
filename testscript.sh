#!/bin/bash
./user 0 "ultra_top_secret" 10 0 "2 25 top_secret" &
./user 0 "ultra_top_secret" 10 0 "1 25 top_secret" &
./user 2 "top_secret" 10 2 "1 10" & 
./user 0 "ultra_top_secret" 10 3 "" &