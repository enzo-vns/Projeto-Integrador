#!/usr/bin/env python3
import http.server
import socketserver
import webbrowser
import os
import sys

PORT = 3000
FRONT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'front-end')

class CustomHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=FRONT_DIR, **kwargs)

    def do_GET(self):
        if self.path == '/' or self.path == '/index.html':
            self.send_response(302)
            self.send_header('Location', '/html/index.html')
            self.end_headers()
            return
        return super().do_GET()

def run():
    os.chdir(FRONT_DIR)
    url = f"http://localhost:{PORT}/html/index.html"
    print("\n============================================================")
    print("   🏨 RECANTO BOSQUE — FRONT-END & SISTEMA DE GESTÃO         ")
    print("============================================================")
    print(f" ✅ Servidor ativo em: http://localhost:{PORT}")
    print(f" 🌐 Acesso direto: {url}")
    print("------------------------------------------------------------")
    print(" Abrindo o navegador automaticamente...")
    print(" Pressione Ctrl + C para encerrar o servidor.")
    print("============================================================\n")

    webbrowser.open(url)

    socketserver.TCPServer.allow_reuse_address = True
    with socketserver.TCPServer(("", PORT), CustomHandler) as httpd:
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nServidor encerrado.")

if __name__ == '__main__':
    run()
