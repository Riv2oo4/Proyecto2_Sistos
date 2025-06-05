# Simulador de Sistemas Operativos

## Descripción
Este proyecto implementa un simulador gráfico de algoritmos de planificación (scheduling) y mecanismos de sincronización de procesos, desarrollado en C++ con la biblioteca wxWidgets. La aplicación ofrece dos módulos principales:

1. **Calendarización de Procesos**  
   - Carga de procesos desde archivo de texto (formato CSV: PID, Burst Time, Arrival Time, Prioridad).  
   - Selección de algoritmo de scheduling:  
     - First In First Out (FIFO)  
     - Shortest Job First (SJF)  
     - Shortest Remaining Time (SRT)  
     - Round Robin (RR) (ajustable con quantum)  
     - Prioridad (non-preemptive)  
   - Visualización dinámica del diagrama de Gantt con scroll automático.  
   - Cálculo y presentación de métricas de eficiencia:  
     - Tiempo medio de espera (Average Waiting Time)  
     - Tiempo medio de retorno (Average Turnaround Time)  
     - Rendimiento (Throughput)  

2. **Sincronización de Recursos**  
   - Carga de procesos, recursos y acciones desde archivos de texto:  
     - Procesos: mismo formato CSV que en calendarización  
     - Recursos: “NombreRecurso, ContadorInicial”  
     - Acciones: “PID, ACCIÓN (READ/WRITE), Recurso, Ciclo”  
   - Selección de modo de sincronización:  
     - Mutex Locks  
     - Semáforos  
   - Representación gráfica de la línea de tiempo:  
     - Cada proceso dibuja un rectángulo en el ciclo correspondiente a su acción (READ/WRITE).  
     - Estados de espera (si el recurso no está disponible) se marcan en color diferenciado.  
     - Liberación automática de recursos en ciclos posteriores, según semántica de mutex o semáforos.  
   - Paleta de colores pastel y texto en negrita para distinguir procesos y recursos.

## Características principales
- Interfaz basada en pestañas (`wxNotebook`), separando claramente los dos módulos.  
- Diseño modular en clases:  
  - `MainFrame` (ventana principal)  
  - `SchedulingPanel` (configuración y vista de algoritmos de scheduling)  
  - `SynchronizationPanel` (configuración y vista de sincronización de recursos)  
  - `GanttChart` (dibujo y animación del diagrama de Gantt)  
  - `TimelineChart` (dibujo y animación de la línea de tiempo de sincronización)  
- Paleta de colores asignada automáticamente a cada proceso para facilitar su identificación.  
- Scroll automático en las vistas gráficas cuando el ciclo de ejecución supera el ancho visible.  
- Validación de carga de archivos con manejo de excepciones y mensajes de error claros.  
- Captura de excepciones al iniciar la aplicación (bloque `try/catch` en `OnInit`) para notificar fallos al usuario sin crash.  
- Makefile optimizado y escalable:
  - Separación de compilación (`.cpp → .o`) y enlace.  
  - Variables descriptivas (`CXXFLAGS`, `LDFLAGS`, `WXCONFIG`, etc.).  
  - Bandeja de banderas de advertencia (`-Wall -Wextra -g`).  
  - Regla genérica `%.o: %.cpp` para admitir múltiples fuentes.  
  - Objetivos “phony” (`all`, `clean`, `distclean`).  
  - Soporte comentado para generación automática de dependencias.

## Requisitos
- **wxWidgets** v3.0 o superior (instalado y configurado en el sistema).  
- **Compilador C++** con soporte de C++11 (p. ej., `g++` o `clang++`).  
- **Make** (GNU Make o equivalente).  
- Entorno de desarrollo para C++ (opcional, pero recomendado).  

## Instalación y compilación

1. **Clonar o descargar el repositorio**  
   ```bash
   git clone <URL-del-repositorio>
   cd <nombre-del-directorio>

2. **Ejecutar el comando Make en la temrinal**  
   ```bash
   Make

3. **Correr archivo**  
   ```bash
   ./nombre_ejecutable
