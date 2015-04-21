using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Globalization;
using System.IO;

namespace xmlreader
{
    class Program
    {
        static void Main(string[] args)
        {           
            XmlTextReader reader = new XmlTextReader("events.xml");
            int eventNum = 0;

            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element)
                    if(reader.Name == "event")
                    {
                        eventNum++;
                        Console.Title = eventNum.ToString();
                        reader.Read();
                        string[] eventValues = reader.Value.Split(new char[0], StringSplitOptions.RemoveEmptyEntries);
                        int ind = 0;
                        while (true)
                        {
                            if (eventValues[ind] == "300015")
                                break;
                            ind++;
                        }
                        var myList = eventValues.ToList();
                        myList.RemoveRange(0, ind);
                        eventValues = myList.ToArray();
                        double[] particle1 = new double[5];
                        double[] particle2 = new double[5];
                        for (int i = 0; i < particle1.Length; i++)
                            particle1[i]=Double.Parse(eventValues[i+6],NumberStyles.Float);
                        for (int i = 0; i < particle2.Length; i++)
                            particle2[i] = Double.Parse(eventValues[i + 19], NumberStyles.Float);
                        outputFile(particle1);
                        outputFile(particle2);
                    }
            }
            Console.WriteLine("Number of events in file: {0}",eventNum);
            Console.ReadLine();
        }
        private static void outputFile(double[] arr)
        {
            string path = "events.csv";
            foreach (var item in arr)
            {
                var toFile = string.Format("{0},",item.ToString());
                File.AppendAllText(path, toFile);
            }

            var newline = string.Format("{0}",Environment.NewLine);
            File.AppendAllText(path, newline);
        }
    }
}
