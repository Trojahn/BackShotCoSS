CPPFLAGS := -O2 -std=c++11 -pthread -Wl,--no-as-needed
LDLIBS := `pkg-config --libs --cflags opencv`
OBJS = Utils.o InputOutput.o FeatureExtraction.o Shot.o SceneSegmentation.o Parameters.o
PROG = BackShotCoSS
CXX = g++

all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) src/main.cpp -o $@ $(LDLIBS)

%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

clean:
	@rm -f $(PROG) *.o