using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.IO.Ports;

namespace Testing_serial_v3._0
{
    class Program
    {

        /*******************************************************************************
         *******************************************************************************/
        static string FormattedNow()
        {
            return DateTime.Now.ToString("yyyyMMdd-HHmmss");
        }



        /*******************************************************************************
        *******************************************************************************/
        static char ReadSingleBoundedKey(string ValidChars)
        {
            // TODO Implement modes (Interactive WITHOUT time out, Interactive WITH time out, Unattended) see UI.ps1
            ConsoleKeyInfo ConsoleKeyIn;
            if(ValidChars.Length == 0)
            {
                Console.BackgroundColor = ConsoleColor.Black;
                Console.ForegroundColor = ConsoleColor.Red;
                string FunctionName = System.Reflection.MethodBase.GetCurrentMethod().Name;
                Console.WriteLine("{0} Exception: {1} ValidChars should not be empty", FormattedNow(), FunctionName);
                Console.ResetColor();
                Environment.Exit(0);
            }

            bool ConsoleKeyInValid = false;
            char ConsoleCharIn = ' ';
            while(ConsoleKeyInValid == false)
            {
                ConsoleKeyIn = Console.ReadKey(true);
                ConsoleCharIn = ConsoleKeyIn.KeyChar;
                for(byte ValidCharsArrayIndex = 0; ValidCharsArrayIndex < ValidChars.Length; ValidCharsArrayIndex++)
                {
                    //Console.WriteLine("Comparing{0} with{1}", ValidChars[ValidCharsArrayIndex], ConsoleCharIn);
                    if(Convert.ToChar(ValidChars[ValidCharsArrayIndex]) == ConsoleCharIn)
                    {
                        //Console.WriteLine("Valid");
                        ConsoleKeyInValid = true;
                        Console.WriteLine("{0} Selection:{1}", FormattedNow(), ConsoleCharIn);
                        break;
                    }
                }
            }
            return ConsoleCharIn;
        }



        /*******************************************************************************
         *******************************************************************************/
        static string LoadFromFile(string SourceFilePath)
        {
            string Result = "";

            StringBuilder sb = new StringBuilder();
            using(StreamReader sr = new StreamReader(SourceFilePath))
            {
                string line = "";
                /* Read and display lines from the file until the end of the file is reached. */
                while((line = sr.ReadLine()) != null)
                {
                    sb.AppendLine(line);
                }
            }
            Result = sb.ToString();

            return Result;
        }



        /*******************************************************************************
         *******************************************************************************/
        static void Main(string[] args)
        {
            const int TerminalHexByteLength = 4;
            string BufferSerialLineIn;
            string SerialPortSelected = "";
            int Idx = 0;

            /* Info Banner */
            Console.WriteLine("zTsunamiPC v0.1 20140714");
            Console.WriteLine("Jean-Louis Bourdon");
            Console.WriteLine("jnls.bourdon@googlemail.com");
            Console.WriteLine("https://code.google.com/p/ztsunami/");

            //TODO firmware version checking (firmware 0.2 will have a different data format and will not be compatible with 0.1)

            Console.WriteLine(" ");

            /* Serial Port Selection */
            string[] SerialPortsList = SerialPort.GetPortNames();
            if(SerialPortsList.Length == 0)
            {
                Console.WriteLine("No Serial Port available, Exiting");
                Environment.Exit(0);
            }

            Console.WriteLine("Serial Ports available:");

            for(Idx = 0; Idx < SerialPortsList.Length; Idx ++)
            {
                Console.WriteLine("\t [{0}] {1}", Idx, SerialPortsList[Idx]);
            }
            Console.WriteLine("\t [q] quit");
            Console.WriteLine("Select:");
            char MainMenuAnswer = ' ';
            MainMenuAnswer = ReadSingleBoundedKey("0123456789q");
            if(MainMenuAnswer == 'q')
            {
                Environment.Exit(0);
            }
            //TODO handle if MainMenuAnswer > SerialPortsList.Length
            //TODO handle already opened with "if (port.IsOpen)"

            SerialPortSelected = SerialPortsList[(int)(MainMenuAnswer - '0')];

            SerialPort CC1110 = new SerialPort(SerialPortSelected, 115200, Parity.None, 8, StopBits.One);
            try
            {
                CC1110.Open();
            }
            catch(Exception ex)
            {
                Console.WriteLine("Error opening CC1110 Serial Port: {0}", ex.Message);
                Environment.Exit(0);
            }

            Console.WriteLine("Awaiting Z-Wave packets ...");

            /* Create Device List File */
            string DeviceFile = "Devices.txt";//TODO make Network Specific to allow multiple networks
            if(!File.Exists(DeviceFile))
            {
                using(StreamWriter DeviceFileStream = File.AppendText(DeviceFile))
                {
                    //TODO print header
                    for(Idx = 0; Idx < 256; Idx++)
                    {
                        string Text = "0x" + Idx.ToString("X") + "\t" + Idx.ToString("D") + "\t";//TODO 0 fill when single hex digit
                        DeviceFileStream.WriteLine(Text);
                    }
                }
            }

            string[] DeviceLine = LoadFromFile(DeviceFile).Split(new string[] { "\r\n", "\n" }, StringSplitOptions.None); //TODO handle comment lines

            while(true)//TODO continue until keystroke, rather than look forever, use threads
            {
                BufferSerialLineIn = CC1110.ReadLine();
                /* Only process line that contain Radio Data In */
                if(BufferSerialLineIn.StartsWith("RxD = "))
                {
                    Console.WriteLine(BufferSerialLineIn);
                    string zwaveNetworkID = BufferSerialLineIn.Substring(6, (TerminalHexByteLength * 4) + 3);
                    string zwaveSource = BufferSerialLineIn.Substring(26, TerminalHexByteLength);
                    string zwaveFrameControl = BufferSerialLineIn.Substring(31, (TerminalHexByteLength * 2) + 1); //TODO Optional routing when FC&0x80
                    string zwaveLength = BufferSerialLineIn.Substring(41, TerminalHexByteLength);

                    int ByteCount = int.Parse(zwaveLength.Substring(2, 2), System.Globalization.NumberStyles.HexNumber);

                    string zwaveChecksum = "";
                    int CheckSumLastCharIdx = 46 + (ByteCount * 5) + 4;

                    if(CheckSumLastCharIdx <= BufferSerialLineIn.Length) /* Make sure it's possible to reach position */
                    {
                        zwaveChecksum = BufferSerialLineIn.Substring(46 + (ByteCount * 5), 4);
                    }
                    else
                    {
                        zwaveChecksum = "??";
                    }
                    //TODO Verify checksum

                    string zwaveDestination = BufferSerialLineIn.Substring(46, TerminalHexByteLength);
                    string zwaveCommandClass = BufferSerialLineIn.Substring(51, TerminalHexByteLength);
                    string zwaveCommand = BufferSerialLineIn.Substring(56, TerminalHexByteLength);
                    string zwaveData = BufferSerialLineIn.Substring(61, TerminalHexByteLength);

                    int DeviceIndex = int.Parse(zwaveDestination.Substring(2, 2), System.Globalization.NumberStyles.HexNumber);
                    string[] DeviceFields = DeviceLine[DeviceIndex].Split('\t');
                    string DestinationName = DeviceFields[2];

                    DeviceIndex = int.Parse(zwaveSource.Substring(2, 2), System.Globalization.NumberStyles.HexNumber);
                    DeviceFields = DeviceLine[DeviceIndex].Split('\t');
                    string SourceName = DeviceFields[2];

                    //TODO move class decoding in a separate .cs file
                    string CommandClassName = "";
                    string CommandName = "";
                    string DataName = "";
                    switch(zwaveCommandClass)
                    {
                        case "0x20": /* d32 */
                            CommandClassName = "BASIC";
                            switch(zwaveCommand)
                            {
                                case "0x01":
                                    CommandName = "SET";
                                    switch(zwaveData)
                                    {
                                        case "0x00":
                                            DataName = "OFF";
                                            break;
                                        case "0xFF":
                                            DataName = "ON";
                                            break;
                                        default:
                                            DataName  = "INVALID";
                                            break;
                                    }
                                    break;
                                case "0x02":
                                    CommandName = "GET";
                                    break;
                                case "0x03":
                                    CommandName = "REPORT";
                                    break;
                                default:
                                    CommandName = "INVALID COMMAND";
                                    break;
                            }
                            break;

                        case "0x25": /* d37 */
                            CommandClassName = "BINARY_SWITCH";
                            switch(zwaveCommand)
                            {
                                case "0x01":
                                    CommandName = "SET";
                                    break;
                                case "0x02":
                                    CommandName = "GET";
                                    break;
                                case "0x03":
                                    CommandName = "REPORT";
                                    break;
                                default:
                                    CommandName = "INVALID COMMAND";
                                    break;
                            }
                            break;

                        case "0x30": /* d48 */
                            CommandClassName = "BINARY_SENSOR";
                            switch(zwaveCommand)
                            {
                                case "0x02":
                                    CommandName = "GET";
                                    break;
                                case "0x03":
                                    CommandName = "REPORT";
                                    break;
                                default:
                                    CommandName = "INVALID COMMAND";
                                    break;
                                //TYPE_GENERAL_PURPOSE    1
                                //TYPE_SMOKE    2
                                //TYPE_CARBON_OXIDE    3
                                //TYPE_CARBON_DIOXIDE    4
                                //TYPE_HEAT    5
                                //TYPE_WATER    6
                                //TYPE_FREEZE    7
                                //TYPE_TAMPER    8
                                //TYPE_AUX    9
                                //TYPE_DOOR_WINDOW    10
                                //TYPE_TILT    11
                                //TYPE_MOTION    12
                                //TYPE_GLASS_BREAK    13
                                //TYPE_FIRST_SUPPORTED    255
                            }
                            break;

                        case "0x46": /* d70 */
                            CommandClassName = "CLIMATE_CONTROL_SCHEDULE";
                            switch(zwaveCommand)
                            {
                                case "0x01":
                                    CommandName = "MONDAY";
                                    break;
                                case "0x02":
                                    CommandName = "TUESDAY";
                                    break;
                                case "0x03":
                                    CommandName = "WEDNESDAY";
                                    break;
                                case "0x04":
                                    CommandName = "THURSDAY";
                                    break;
                                case "0x05":
                                    CommandName = "FRIDAY";
                                    break;
                                case "0x06":
                                    CommandName = "SATURDAY";
                                    break;
                                case "0x07":
                                    CommandName = "SUNDAY";
                                    break;
                                default:
                                    CommandName = "INVALID COMMAND";
                                    break;
                            }
                            break;

                        case "0x70": /* d112 */
                            CommandClassName = "CONFIGURATION";
                            switch(zwaveCommand)
                            {
                                case "0x04":
                                    CommandName = "SET";
                                    break;
                                case "0x05":
                                    CommandName = "GET";
                                    break;
                                case "0x06":
                                    CommandName = "REPORT";
                                    break;
                                default:
                                    CommandName = "INVALID COMMAND";
                                    break;
                                //VAR_LEN_1    1
                                //VAR_LEN_2    2
                                //VAR_LEN_4    4
                            }
                            break;

                        case "0x80": /* d128 */
                            CommandClassName = "BATTERY";
                            switch(zwaveCommand)
                            {
                                case "0x02":
                                    CommandName = "GET";
                                    break;
                                case "0x03":
                                    CommandName = "REPORT";
                                    break;
                                default:
                                    CommandName = "INVALID COMMAND ";
                                    break;
                            }
                            break;

                        case "0x81": /* d129 */
                            CommandClassName = "CLOCK";
                            switch(zwaveCommand)
                            {
                                case "0x01":
                                    CommandName = "CLOCK_VERSION";
                                    break;
                                case "0x02":
                                    CommandName = "TUESDAY";
                                    break;
                                case "0x03":
                                    CommandName = "WEDNESDAY";
                                    break;
                                case "0x04":
                                    CommandName = "CLOCK_SET";
                                    break;
                                case "0x05":
                                    CommandName = "CLOCK_GET";
                                    break;
                                case "0x06":
                                    CommandName = "CLOCK_REPORT";
                                    break;
                                default:
                                    CommandName = "INVALID COMMAND";
                                    break;
                                //HOUR_MIN    0
                                //HOUR_MAX    23
                                //MINUTE_MIN    0
                                //MINUTE_MAX    59
                                //WEEKDAY_UNUSED    0
                                //MONDAY    1
                                //TUESDAY    2
                                //WEDNESDAY    3
                                //THURSDAY    4
                                //FRIDAY    5
                                //SATURDAY    6
                                //SUNDAY    7
                            }
                            break;

                        case "0x85": /* d133 */
                            CommandClassName = "ASSOCIATION";
                            switch(zwaveCommand)
                            {
                                case "0x01":
                                    CommandName = "SET";
                                    break;
                                case "0x02":
                                    CommandName = "GET";
                                    break;
                                case "0x03":
                                    CommandName = "REPORT";
                                    break;
                                case "0x04":
                                    CommandName = "REMOVE";
                                    break;
                                case "0x05":
                                    CommandName = "GROUPING_GET";
                                    break;
                                case "0x06":
                                    CommandName = "GROUPING_REPORT";
                                    break;
                                case "0x0B":
                                    CommandName = "SPECIFIC_GROUP_GET_V2";
                                    break;
                                case "0x0C":
                                    CommandName = "SPECIFIC_GROUP_REPORT_V2";
                                    break;
                                default:
                                    CommandName = "INVALID COMMAND";
                                    break;
                            }
                            break;

                        case "0x8E": /* d142 */
                            CommandClassName = "MULTI_CHANNEL_ASSOCIATION"; // or MULTIINSTANCE_ASSOCIATION
                            CommandName = "INVALID COMMAND";
                            break;

                        case "0x60": /* d96 */
                            CommandClassName = "MULTIINSTANCE"; // or "MULTICHANNEL";
                            switch(zwaveCommand)
                            {
                                case "0x04":
                                    CommandName = "GET"; //(MultiInstanceCmd)
                                    break;
                                case "0x05":
                                    CommandName = "REPORT"; //(MultiInstanceCmd)
                                    break;
                                case "0x06":
                                    CommandName = "ENCAP"; //(MultiInstanceCmd)
                                    break;
                                case "0x07":
                                    CommandName = "END_POINT_GET"; //(MultiInstanceCmd)
                                    break;
                                case "0x08":
                                    CommandName = "END_POINT_REPORT"; //(MultiChannelCmd)
                                    break;
                                case "0x09":
                                    CommandName = "CAPABILITY_GET"; //(MultiChannelCmd)
                                    break;
                                case "0x0A":
                                    CommandName = "CAPABILITY_REPORT"; //(MultiChannelCmd)
                                    break;
                                case "0x0B":
                                    CommandName = "END_POINT_FIND"; //(MultiChannelCmd)
                                    break;
                                case "0x0C":
                                    CommandName = "END_POINT_REPORT"; //(MultiChannelCmd)
                                    break;
                                case "0x0D":
                                    CommandName = "ENCAP"; //(MultiChannelCmd)
                                    break;
                                default:
                                    CommandName = "INVALID COMMAND ";
                                    break;
                            }
                            break;

                        case "0x26": /* d38 */
                            CommandClassName = "MULTILEVEL_SWITCH";
                            switch(zwaveCommand)
                            {
                                case "0x01":
                                    CommandName = "SET";
                                    break;
                                case "0x02":
                                    CommandName = "GET";
                                    break;
                                case "0x03":
                                    CommandName = "REPORT";
                                    break;
                                default:
                                    CommandName = "INVALID COMMAND ";
                                    break;
                            }
                            break;

                        case "0x73": /* d115 */
                            CommandClassName = "POWERLEVEL";
                            switch(zwaveCommand)
                            {
                                case "0x01":
                                    CommandName = "SET";
                                    break;
                                case "0x02":
                                    CommandName = "GET";
                                    break;
                                case "0x03":
                                    CommandName = "REPORT";
                                    break;
                                case "0x04":
                                    CommandName = "TEST_NODE_SET";
                                    break;
                                case "0x05":
                                    CommandName = "TEST_NODE_GET";
                                    break;
                                case "0x06":
                                    CommandName = "TEST_NODE_REPORT";
                                    break;
                                default:
                                    CommandName = "INVALID COMMAND ";
                                    break;
                                //VERSION    1
                            }
                            break;

                        default:
                            CommandClassName = "INVALID CLASS";
                            CommandName = "INVALID COMMAND";
                            break;
                    }

                    /* Log to a file */
                    string SorterLogPath = zwaveNetworkID + "_log.txt";
                    using(StreamWriter LogStream = File.AppendText(SorterLogPath))
                    {
                        string Text = FormattedNow() + ": " + BufferSerialLineIn;
                        LogStream.Write(Text);
                        Text = FormattedNow() + ": " + "Src: " + zwaveSource + " FCt: " + zwaveFrameControl + " Len: " + zwaveLength + " Dst: " + zwaveDestination + " Cls: " + zwaveCommandClass + " Com: " + zwaveCommand + " Dat: " + zwaveData + " CkS: " + zwaveChecksum + "\r\n";
                        LogStream.Write(Text);
                        Text = FormattedNow() + ": " + SourceName + " -> " + DestinationName + ", " + CommandClassName + ", " + CommandName + " " + DataName + "\r\n";
                        LogStream.Write(Text);
                        LogStream.WriteLine("");
                    }
                }
            }
        }
    }
}