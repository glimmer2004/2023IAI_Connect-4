root_dir="/home/glimmer/connect-4/Linux/"  #your path here
for file in `ls "${root_dir}Sourcecode"`
do
    cd "${root_dir}Sourcecode/${file}"
    pwd
    g++ -Wall -std=c++11 -O2 -fpic -shared *.cpp -o "${root_dir}/so/${file}.so"
    cd -
done
