# -------------------------------------------------------------
# Variables principales
# -------------------------------------------------------------
CXX        := g++
WXCONFIG   := wx-config
CXXFLAGS   := -std=c++11 -Wall -Wextra -g $(shell $(WXCONFIG) --cxxflags)
LDFLAGS    := $(shell $(WXCONFIG) --libs)
TARGET     := simulator

# Lista de archivos fuente .cpp 
SRCS       := main.cpp
# Obtenemos los .o correspondientes
OBJS       := $(SRCS:.cpp=.o)

# -------------------------------------------------------------
# Regla por defecto
# -------------------------------------------------------------
.PHONY: all
all: $(TARGET)

# -------------------------------------------------------------
# Cómo generar el ejecutable a partir de los .o
# -------------------------------------------------------------
$(TARGET): $(OBJS)
	@echo "Linkeando: $@"
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# -------------------------------------------------------------
# Regla genérica para compilar cada .cpp a .o
# -------------------------------------------------------------
# $<  = primer prerequisito 
# $@  = nombre del objetivo 
# $^  = lista de prerequisitos 
%.o: %.cpp
	@echo "Compilando: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -------------------------------------------------------------
# Limpiar objetos y ejecutable
# -------------------------------------------------------------
.PHONY: clean
clean:
	@echo "Eliminando ejecutable y archivos .o..."
	rm -f $(TARGET) $(OBJS)

.PHONY: distclean
distclean: clean
	@echo "Eliminando dependencias generadas..."
	rm -f *.d


