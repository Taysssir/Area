protoc models.proto --cpp_out=. 
cp models.pb.h ../include/eos/configuration/
cp models.pb.cc ../source/models/

