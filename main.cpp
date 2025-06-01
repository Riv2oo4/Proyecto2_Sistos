#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/grid.h>
#include <wx/spinctrl.h>
#include <wx/filedlg.h>
#include <wx/scrolwin.h>
#include <wx/timer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <vector>
#include <string>

class MainFrame;
class GanttChart;
class TimelineChart;

// Estructuras de datos
struct Process {
    wxString pid;
    int burstTime;
    int arrivalTime;
    int priority;
    wxColour color;
};

struct Resource {
    wxString name;
    int counter;
};

struct Action {
    wxString pid;
    wxString action;
    wxString resource;
    int cycle;
};

// Ventana principal
class OSSimulatorApp : public wxApp {
public:
    bool OnInit() override;
};

// Panel para visualización diagrama de  Gantt
class GanttChart : public wxScrolledWindow {
public:
    GanttChart(wxWindow* parent);
    void StartSimulation();
    void StopSimulation();
    void ResetChart();
    void SetProcesses(const std::vector<Process>& processes);
    
private:
    void OnPaint(wxPaintEvent& event);
    void OnTimer(wxTimerEvent& event);
    void DrawTimeAxis(wxPaintDC& dc);
    void DrawProcessBlocks(wxPaintDC& dc);
    
    wxTimer* m_timer;
    std::vector<Process> m_processes;
    int m_currentCycle;
    bool m_isRunning;
    
    wxDECLARE_EVENT_TABLE();
};

// Panel para timeline de sincronización
class TimelineChart : public wxScrolledWindow {
public:
    TimelineChart(wxWindow* parent);
    void StartSimulation();
    void StopSimulation();
    void ResetChart();
    void SetData(const std::vector<Process>& processes, 
                 const std::vector<Resource>& resources,
                 const std::vector<Action>& actions);
    
private:
    void OnPaint(wxPaintEvent& event);
    void OnTimer(wxTimerEvent& event);
    void DrawTimeline(wxPaintDC& dc);
    
    wxTimer* m_timer;
    std::vector<Process> m_processes;
    std::vector<Resource> m_resources;
    std::vector<Action> m_actions;
    int m_currentCycle;
    bool m_isRunning;
    
    wxDECLARE_EVENT_TABLE();
};

// Panel de calendarización
class SchedulingPanel : public wxPanel {
public:
    SchedulingPanel(wxWindow* parent);
    
private:
    void OnLoadProcesses(wxCommandEvent& event);
    void OnStartSimulation(wxCommandEvent& event);
    void OnStopSimulation(wxCommandEvent& event);
    void OnResetSimulation(wxCommandEvent& event);
    void OnAlgorithmCheck(wxCommandEvent& event);
    void OnQuantumChange(wxSpinEvent& event);
    void LoadProcessesFromFile(const wxString& filename);
    void UpdateMetrics();
    
    wxCheckBox* m_fifoCheck;
    wxCheckBox* m_sjfCheck;
    wxCheckBox* m_srtCheck;
    wxCheckBox* m_rrCheck;
    wxCheckBox* m_priorityCheck;
    wxSpinCtrl* m_quantumSpin;
    wxButton* m_loadProcessesBtn;
    wxButton* m_startBtn;
    wxButton* m_stopBtn;
    wxButton* m_resetBtn;
    wxListCtrl* m_processListCtrl;
    wxGrid* m_metricsGrid;
    GanttChart* m_ganttChart;
    
    std::vector<Process> m_processes;
    
    wxDECLARE_EVENT_TABLE();
};
