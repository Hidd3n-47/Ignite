using System.Threading;

public static class ConsoleHost
{
    private static DebugConsole.MainWindow window;

    public static void Start()
    {
        if (window == null)
        {
            var thread = new Thread(() =>
            {
                window = new DebugConsole.MainWindow();
                window.Show();
                System.Windows.Threading.Dispatcher.Run();
            });
            thread.SetApartmentState(ApartmentState.STA);
            thread.IsBackground = true;
            thread.Start();
        }
    }

    public static void Stop()
    {
        if (window != null)
        {
            window.Dispatcher.Invoke(() =>
            {
                window.Close();
            });
            window = null;
        }
    }
}
