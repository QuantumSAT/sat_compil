#Author: Juexiao Su
#Purpose: Test basic functionality

puts "#########################################"
puts "#        read blif netlist              #"
puts "#########################################"
read_blif 3gate.blif
gen_dwave_nl
puts "\n"

puts "#########################################"
puts "#     initialize hardware target        #"
puts "#########################################"
init_target -row 3 -col 3 -local 8
write_blif 3gate_dwave.blif
puts "\n"

puts "#########################################"
puts "#     initialize place and route        #"
puts "#########################################"
init_system 
puts "\n"

puts "#########################################"
puts "#           place netlist               #"
puts "#########################################"
place

