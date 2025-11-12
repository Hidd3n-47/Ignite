using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Input;

namespace DebugConsole
{
    public partial class MainWindow : Window
    {
        private List<string> allLogs = new List<string>();

        public MainWindow()
        {
            InitializeComponent();

            AddLog("This is a testing log message");
        }

        public void AddLog(string message)
        {
            string timestamp = DateTime.Now.ToString("HH:mm:ss");
            string entry = $"[{timestamp}] {message}";
            allLogs.Add(entry);
            ApplyFilter();
        }

        private void FilterTextBox_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
            ApplyFilter();
        }

        private void ApplyFilter()
        {
            //string filter = FilterTextBox.Text.Trim().ToLower();
            //var filtered = string.IsNullOrEmpty(filter)
            //    ? allLogs
            //    : allLogs.Where(log => log.ToLower().Contains(filter)).ToList();

            //LogListBox.ItemsSource = filtered;
        }

        private void Window_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left)
                this.DragMove();
        }
    }
}
