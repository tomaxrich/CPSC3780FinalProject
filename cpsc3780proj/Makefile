## CC = g++
## export BASE_DIR = /home/wolf/Git/cpsc3780proj
BASE_DIR := $(CURDIR)
## CCFLAGS = -Wall
## BJS = server/ServerS1.o
export BASE_OBJ_DIR := $(BASE_DIR)/obj
export BASE_SRC_DIR := $(BASE_DIR)/src
export BASE_BIN_DIR := $(BASE_DIR)/bin

CC = g++
CCFLAGS = -Wall -g -I$(BASE_SRC_DIR) -I$(BASE_SRC_DIR)/common -pthread
COMMON_OBJS = common/Common.o common/Frame.o common/Socket.o
SERVER_OBJS = server/ServerSocket.o $(COMMON_OBJS)
SERVER_NONFILE = pre_build
SERVER_1_SPECIFIC = server/ServerS1.o server/Protocol.o
SERVER_2_SPECIFIC = server/ServerS2.o server/Protocol_s2.o
CLIENT_OBJS = client/ClientSocket.o $(COMMON_OBJS)
CLIENT_1_SPECIFIC = client/ClientScenario1.o $(CLIENT_OBJS)
CLIENT_2_SPECIFIC = client/ClientScenario2.o $(CLIENT_OBJS)
CLIENT_NONFILE = pre_build

OBJS = $(SERVER_OBJS)

.PHONY : build-all
build-all: pre_build server_s1 client_s1 server_s2

.PHONY : pre_build
pre_build: 
	mkdir -p $(BASE_OBJ_DIR)
	mkdir -p $(BASE_BIN_DIR)

server_s1: $(SERVER_NONFILE) $(addprefix $(BASE_SRC_DIR)/,$(SERVER_OBJS)) $(addprefix $(BASE_SRC_DIR)/,$(SERVER_1_SPECIFIC))
	$(CC) $(CCFLAGS) $(subst $(BASE_SRC_DIR),$(BASE_OBJ_DIR),$(subst $(SERVER_NONFILE),,$^)) -o $(BASE_BIN_DIR)/$@

server_s2: $(SERVER_NONFILE) $(addprefix $(BASE_SRC_DIR)/,$(SERVER_OBJS)) $(addprefix $(BASE_SRC_DIR)/,$(SERVER_2_SPECIFIC))
	$(CC) $(CCFLAGS) $(subst $(BASE_SRC_DIR),$(BASE_OBJ_DIR),$(subst $(SERVER_NONFILE),,$^)) -o $(BASE_BIN_DIR)/$@

client_s1: $(CLIENT_NONFILE) $(addprefix $(BASE_SRC_DIR)/,$(CLIENT_OBJS)) $(addprefix $(BASE_SRC_DIR)/,$(CLIENT_1_SPECIFIC))
	$(CC) $(CCFLAGS) $(subst $(BASE_SRC_DIR),$(BASE_OBJ_DIR),$(subst $(CLIENT_NONFILE),,$^)) -o $(BASE_BIN_DIR)/$@

client_s2: $(CLIENT_NONFILE) $(addprefix $(BASE_SRC_DIR)/,$(CLIENT_OBJS)) $(addprefix $(BASE_SRC_DIR)/,$(CLIENT_2_SPECIFIC))
	$(CC) $(CCFLAGS) $(subst $(BASE_SRC_DIR),$(BASE_OBJ_DIR),$(subst $(CLIENT_NONFILE),,$^)) -o $(BASE_BIN_DIR)/$@

%.o : %.cpp
	mkdir -p $(BASE_OBJ_DIR)$(dir $(subst $(BASE_SRC_DIR),,$@))
	$(CC) $(CCFLAGS) -c $< -o $(addprefix $(BASE_OBJ_DIR),$(subst $(BASE_SRC_DIR),,$@))

## generate the prerequistes and append to the desired file
.prereq : $(addprefix $(BASE_SRC_DIR)/,$(OBJS:.o=.cpp)) $(wildcard *.h) Makefile
	rm -f .prereq
	$(CC) $(CCFLAGS) -MM $(addprefix $(BASE_SRC_DIR)/,$(OBJS:.o=.cpp)) >> ./.prereq

## include the generated prerequisite file
include .prereq

.PHONY : clean
clean :
	rm -f *.o *~ #* .#*
## 	rm -f .prereq
	find . -name ".prereq" -type f -delete

.PHONY : clean-all
clean-all : clean
	rm -f -r $(BASE_OBJ_DIR)
	rm -f -r $(BASE_BIN_DIR)
