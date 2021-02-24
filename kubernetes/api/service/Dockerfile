FROM python:3.7

WORKDIR /code  
COPY . /code/
RUN pip install -r requirements.txt  

EXPOSE 5001  
CMD ["python", "/code/api.py"]`
