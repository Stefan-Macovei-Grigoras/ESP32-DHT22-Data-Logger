import { useEffect, useState } from "react";
import "bootstrap/dist/css/bootstrap.min.css";

const Dashboard = () => {
  const navigate = useNavigate();
  const [sensorData, setSensorData] = useState({ temperature: "--", humidity: "--" });

  useEffect(() => {
    const ws = new WebSocket("ws://localhost:3000");

    ws.onmessage = (event) => {
      const data = JSON.parse(event.data);
      setSensorData(data);
    };

    return () => ws.close();
  }, []);

  return (
    <div className="container-fluid vh-100 p-4 d-flex">
      <div className="p-4 d-flex align-items-center justify-content-center flex-fill">
        <div className="border rounded p-4 bg-light shadow w-100 text-center user-select-none">
          <h2 className="fw-bold mb-3">Sensor Data</h2>
          <p><strong>Temperature:</strong> {sensorData.temperature}°C</p>
          <p><strong>Humidity:</strong> {sensorData.humidity}%</p>
        </div>
      </div>
    </div>
  );
};

export default Dashboard;
