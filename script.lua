arg1_lower = string.lower( arg[ 1 ] )
arg2_lower = string.lower( arg[ 2 ] )

if string.find( arg1_lower, arg2_lower )
	then return true
	else return false
end

--if string.find( ) returns nil, then return false else return true
--string.find( ) returns either nil or an integer
