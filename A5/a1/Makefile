CXX= g++
CXXFLAGS = -g

all: addpeer removepeer addcontent removecontent lookupcontent

addpeer:
	$(CXX) src/addpeer/*  $(CXXFLAGS) -o addpeer

removepeer:
	$(CXX) src/removepeer/*  $(CXXFLAGS) -o removepeer

addcontent:
	$(CXX) src/addcontent/*  $(CXXFLAGS) -o addcontent

removecontent:
	$(CXX) src/removecontent/*  $(CXXFLAGS) -o removecontent

lookupcontent:
	$(CXX) src/lookupcontent/*  $(CXXFLAGS) -o lookupcontent

clean:
	rm -rf addpeer removepeer addcontent removecontent lookupcontent *.o
