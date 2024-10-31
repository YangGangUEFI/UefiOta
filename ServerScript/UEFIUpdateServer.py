import http.server
import socketserver
import json
import os
import sys
from urllib.parse import parse_qs
import threading
import cgi
import socket

def get_local_ip():
    """获取本机IPv4地址"""
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
        s.close()
        return ip
    except Exception:
        return "127.0.0.1"

# 全局变量存储发布状态和IP地址
published_data = {
    'is_published': False,
    'version': None,
    'file_path': None
}
LOCAL_IP = get_local_ip()

HTML = """
<!DOCTYPE html>
<html>
<head>
    <title>UEFI BIOS Update Server</title>
    <style>
        body { 
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 0 auto;
            padding: 20px;
        }
        h1 { text-align: center; }
        .container {
            display: flex;
            flex-direction: column;
            gap: 20px;
        }
        .form-group {
            display: flex;
            flex-direction: column;
            gap: 5px;
        }
        button {
            padding: 10px;
            cursor: pointer;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
        }
        button:disabled {
            background-color: #cccccc;
            cursor: not-allowed;
        }
        button#stopBtn {
            background-color: #f44336;
        }
        .message {
            padding: 10px;
            margin-top: 10px;
            border-radius: 4px;
        }
        .success { background-color: #dff0d8; }
        .error { background-color: #f2dede; }
        #filePath {
            word-break: break-all;
            margin-top: 5px;
            font-size: 0.9em;
            color: #666;
        }
        .status-section {
            margin-top: 30px;
            padding: 15px;
            background-color: #f8f9fa;
            border-radius: 4px;
        }
        .status-title {
            font-size: 1.2em;
            font-weight: bold;
            margin-bottom: 10px;
        }
        .status-info {
            font-family: monospace;
            white-space: pre-wrap;
            word-break: break-all;
            background-color: white;
            padding: 10px;
            border-radius: 4px;
            border: 1px solid #ddd;
        }
        .no-publish {
            color: #666;
            font-style: italic;
        }
    </style>
</head>
<body>
    <h1>UEFI BIOS Update Server</h1>
    <div class="container">
        <div class="form-group">
            <label for="version">Version:</label>
            <input type="text" id="version" placeholder="Enter version (e.g., 1.0.0)" required>
        </div>
        <div class="form-group">
            <label for="fileInput">Select BIOS File:</label>
            <input type="file" id="fileInput" required>
            <div id="filePath"></div>
        </div>
        <button id="publishBtn" disabled>Publish</button>
        <button id="stopBtn">Stop Publishing</button>
        <div id="message"></div>
        
        <div class="status-section">
            <div class="status-title">Current Published Status</div>
            <div id="currentStatus" class="status-info no-publish">No BIOS currently published</div>
        </div>
    </div>
    <script>
        const version = document.getElementById('version');
        const fileInput = document.getElementById('fileInput');
        const publishBtn = document.getElementById('publishBtn');
        const stopBtn = document.getElementById('stopBtn');
        const message = document.getElementById('message');
        const filePath = document.getElementById('filePath');
        const currentStatus = document.getElementById('currentStatus');

        async function updateStatus() {
            try {
                const response = await fetch('/status');
                const status = await response.json();
                if (status.is_published) {
                    currentStatus.classList.remove('no-publish');
                    currentStatus.innerHTML = `Version: ${status.version}\nFile Path: ${status.file_path}`;
                } else {
                    currentStatus.classList.add('no-publish');
                    currentStatus.textContent = 'No BIOS currently published';
                }
            } catch (error) {
                currentStatus.textContent = 'Error fetching status';
            }
        }

        // 页面加载时获取状态
        updateStatus();

        function validateInputs() {
            publishBtn.disabled = !version.value || !fileInput.files[0];
        }

        version.addEventListener('input', validateInputs);
        fileInput.addEventListener('change', () => {
            validateInputs();
            if (fileInput.files[0]) {
                filePath.textContent = `Selected file: ${fileInput.files[0].name}`;
            } else {
                filePath.textContent = '';
            }
        });

        publishBtn.addEventListener('click', async () => {
            const formData = new FormData();
            formData.append('version', version.value);
            formData.append('file', fileInput.files[0]);

            try {
                const response = await fetch('/publish', {
                    method: 'POST',
                    body: formData
                });
                const result = await response.text();
                message.className = 'message success';
                message.textContent = result;
                // 更新状态显示
                updateStatus();
            } catch (error) {
                message.className = 'message error';
                message.textContent = 'Error publishing BIOS update';
            }
        });

        stopBtn.addEventListener('click', async () => {
            try {
                const response = await fetch('/stop', {method: 'POST'});
                const result = await response.text();
                message.className = 'message success';
                message.textContent = result;
                // 更新状态显示
                updateStatus();
            } catch (error) {
                message.className = 'message error';
                message.textContent = 'Error stopping BIOS update service';
            }
        });
    </script>
</body>
</html>
"""

class RequestHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/':
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(HTML.encode())
        elif self.path == '/update':
            if published_data['is_published']:
                response = {
                    "message": f"New BIOS version available: {published_data['version']}",
                    "image_url": f"http://{LOCAL_IP}:{PORT}/BIN/{os.path.basename(published_data['file_path'])}"
                }
                self.send_response(200)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps(response).encode())
            else:
                self.send_error(404, "No BIOS update currently published")
        elif self.path == '/status':
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps(published_data).encode())
        else:
            super().do_GET()

    def do_POST(self):
        if self.path == '/publish':
            form = cgi.FieldStorage(
                fp=self.rfile,
                headers=self.headers,
                environ={'REQUEST_METHOD': 'POST',
                        'CONTENT_TYPE': self.headers['Content-Type']}
            )

            version = form['version'].value
            file_item = form['file']
            
            # 保存上传的文件
            file_path = os.path.join(os.getcwd(), "BIN", file_item.filename)
            with open(file_path, 'wb') as f:
                f.write(file_item.file.read())

            published_data.update({
                'is_published': True,
                'version': version,
                'file_path': file_path
            })

            self.send_response(200)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(b"BIOS update published successfully")

        elif self.path == '/stop':
            published_data.update({
                'is_published': False,
                'version': None,
                'file_path': None
            })
            
            self.send_response(200)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(b"BIOS update service stopped")

def run_server(port):
    global PORT
    PORT = port
    
    handler = RequestHandler
    with socketserver.TCPServer(("", port), handler) as httpd:
        print(f"Server started at http://{LOCAL_IP}:{port}")
        print("Press Ctrl+C to stop the server")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nShutting down server...")
            httpd.shutdown()
            httpd.server_close()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 server.py <port>")
        sys.exit(1)
        
    try:
        port = int(sys.argv[1])
        run_server(port)
    except ValueError:
        print("Error: Port must be a number")
        sys.exit(1)