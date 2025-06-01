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

// Panel de sincronización
class SynchronizationPanel : public wxPanel {
public:
    SynchronizationPanel(wxWindow* parent);
    
private:
    void OnLoadProcesses(wxCommandEvent& event);
    void OnLoadResources(wxCommandEvent& event);
    void OnLoadActions(wxCommandEvent& event);
    void OnStartSimulation(wxCommandEvent& event);
    void OnStopSimulation(wxCommandEvent& event);
    void OnResetSimulation(wxCommandEvent& event);
    void OnSyncModeChange(wxCommandEvent& event);
    void LoadProcessesFromFile(const wxString& filename);
    void LoadResourcesFromFile(const wxString& filename);
    void LoadActionsFromFile(const wxString& filename);
    void CheckEnableStart();  
    
    wxChoice* m_syncModeChoice;
    wxButton* m_loadProcessesBtn;
    wxButton* m_loadResourcesBtn;
    wxButton* m_loadActionsBtn;
    wxButton* m_startBtn;
    wxButton* m_stopBtn;
    wxButton* m_resetBtn;
    wxListCtrl* m_processListCtrl;
    wxListCtrl* m_resourceListCtrl;
    wxListCtrl* m_actionListCtrl;
    TimelineChart* m_timelineChart;
    
    std::vector<Process> m_processes;
    std::vector<Resource> m_resources;
    std::vector<Action> m_actions;
    
    wxDECLARE_EVENT_TABLE();
};

// Frame principal
class MainFrame : public wxFrame {
public:
    MainFrame();
    
private:
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNotebookPageChanged(wxBookCtrlEvent& event);
    
    wxNotebook* m_notebook;
    SchedulingPanel* m_schedulingPanel;
    SynchronizationPanel* m_syncPanel;
    
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, MainFrame::OnNotebookPageChanged)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(GanttChart, wxScrolledWindow)
    EVT_PAINT(GanttChart::OnPaint)
    EVT_TIMER(wxID_ANY, GanttChart::OnTimer)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(TimelineChart, wxScrolledWindow)
    EVT_PAINT(TimelineChart::OnPaint)
    EVT_TIMER(wxID_ANY, TimelineChart::OnTimer)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(SchedulingPanel, wxPanel)
    EVT_BUTTON(1001, SchedulingPanel::OnLoadProcesses)
    EVT_BUTTON(1002, SchedulingPanel::OnStartSimulation)
    EVT_BUTTON(1003, SchedulingPanel::OnStopSimulation)
    EVT_BUTTON(1004, SchedulingPanel::OnResetSimulation)
    EVT_CHECKBOX(wxID_ANY, SchedulingPanel::OnAlgorithmCheck)
    EVT_SPINCTRL(1005, SchedulingPanel::OnQuantumChange)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(SynchronizationPanel, wxPanel)
    EVT_BUTTON(2001, SynchronizationPanel::OnLoadProcesses)
    EVT_BUTTON(2002, SynchronizationPanel::OnLoadResources)
    EVT_BUTTON(2003, SynchronizationPanel::OnLoadActions)
    EVT_BUTTON(2004, SynchronizationPanel::OnStartSimulation)
    EVT_BUTTON(2005, SynchronizationPanel::OnStopSimulation)
    EVT_BUTTON(2006, SynchronizationPanel::OnResetSimulation)
    EVT_CHOICE(2007, SynchronizationPanel::OnSyncModeChange)
wxEND_EVENT_TABLE()


// Implementaciones

bool OSSimulatorApp::OnInit() {
    MainFrame* frame = new MainFrame();
    frame->Show(true);
    return true;
}

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Simulador de Sistemas Operativos", 
                                 wxDefaultPosition, wxSize(1200, 800)) {
    // Menu bar
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "E&xit\tCtrl-Q", "Quit this program");
    
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");
    
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);
    
    // Status bar
    CreateStatusBar(2);
    SetStatusText("Listo para simular", 0);
    
    // Notebook principal
    m_notebook = new wxNotebook(this, wxID_ANY);
    
    // Crear paneles
    m_schedulingPanel = new SchedulingPanel(m_notebook);
    m_syncPanel = new SynchronizationPanel(m_notebook);
    
    m_notebook->AddPage(m_schedulingPanel, "A. Calendarización", true);
    m_notebook->AddPage(m_syncPanel, "B. Sincronización", false);
    
    // Layout principal
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(m_notebook, 1, wxEXPAND | wxALL, 5);
    SetSizer(mainSizer);
    
    Centre();
}
