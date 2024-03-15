using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace DWM1000_Koordinat_Tespit
{
	public partial class Form1 : Form
	{
		private SerialPort sensport;
		private string veri;

		public Form1()
		{
			InitializeComponent();
		}

		float map(float val, float iMin, float iMax, float oMin, float oMax)
		{

			return (val - iMin) * (oMax - oMin) / (iMax - iMin) + oMin;
		}

		private void Form1_Load(object sender, EventArgs e)
		{
			comboBox1.DataSource = SerialPort.GetPortNames();

			//pictureBox2.Location = new Point(110, 322);  //Sanal sıfır noktası

			//pictureBox2.Location = new Point(430, 322); //sağ alt köşe

			//pictureBox2.Location = new Point(110, 90);  //sol üst köşe

			//pictureBox2.Location = new Point(430, 90); //sağ üst köşe



			/*
			private static int map(int value, int fromLow, int fromHigh, int toLow, int toHigh) 
			{
				return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
			}
			*/
			
			comboBox2.SelectedIndex= 1;
			
		}

		private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
		{
			
		}

		private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
		{
			sensport = new SerialPort();
			sensport.BaudRate = int.Parse(comboBox2.Text);
			sensport.PortName= comboBox1.Text;
			sensport.Parity = Parity.None;
			sensport.DataBits= 8;
			sensport.StopBits = StopBits.One;
			sensport.Handshake= Handshake.None;

			
		}

		private void button1_Click(object sender, EventArgs e)
		{
			sensport.Open();
			timer1.Start();

		}

		private void button2_Click(object sender, EventArgs e)
		{
			sensport.Close();
			timer1.Stop();
		}

		private void timer1_Tick(object sender, EventArgs e)
		{
			veri = sensport.ReadLine();
			Gveri.Text = veri;

			string[] koordinat = veri.Split('-');

			var stringValue = koordinat[0];
			var number = 0;
			number = int.Parse(stringValue);

			var stringValue1 = koordinat[1];
			var number1 = 0;
			number1 = int.Parse(stringValue1);



			textBox1.Text = koordinat[0];
			textBox2.Text = koordinat[1];



			pictureBox2.Location = new Point(110 + number, 322 - number1);


		}


		
	}
}
