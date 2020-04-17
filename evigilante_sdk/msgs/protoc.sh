protoc msgs.proto --cpp_out=. 
#protoc msgs.proto --python_out=.
cp msgs.pb.h ../include/eos/sdk/
cp msgs.pb.cc ../src/engine/

