##########################################################
##  Henrique Oliveira 2022211169              
##  André Silva 2020228054                    
##  Denzel Bila 2021244709 
##########################################################


from flask import Flask, request, jsonify
import psycopg2
import jwt
import datetime
import logging
import time
import random
from datetime import date, timedelta

app = Flask(__name__)
app.config['SECRET_KEY'] = 'your_secret_key'

##########################################################
## DATABASE ACCESS
##########################################################

def db_connection():
    return psycopg2.connect(
        user='aulaspl',
        password='henriqueoliveira',
        host='127.0.0.1',
        port='5432',
        database='proj'
    )


##########################################################
## ENDPOINTS
##########################################################

def generate_token(user_id):
    token = jwt.encode(
        {'user_id': user_id, 'exp': datetime.datetime.utcnow() + datetime.timedelta(hours=1)},
        app.config['SECRET_KEY'],
        algorithm='HS256'
    )
    return token


def insert_employee(data, user_id, cur):
    query = '''INSERT INTO employee (salary, users_user_id) 
               VALUES (%s, %s) RETURNING users_user_id'''
    cur.execute(query, (data['salary'], user_id))
    return cur.fetchone()[0]


def insert_common_user(data, cur):
    query = '''INSERT INTO users (user_id, username, password, email, conactinfo) 
               VALUES (%s ,%s, %s, %s, %s) RETURNING user_id'''
    cur.execute(query, (data['user_id'], data['username'], data['password'], data['email'], data['conactinfo']))
    return cur.fetchone()[0]

def insert_patient(data,cur):
    query = '''INSERT INTO patient (users_user_id) 
               VALUES (%s) RETURNING users_user_id'''
    cur.execute(query, (data['user_id'],))
    return cur.fetchone()[0]

@app.route('/Projeto/register/patient', methods=['POST'])
def register_patient():
    data = request.get_json()
    conn = db_connection() 
    cur = conn.cursor()
    try:
        user_id = insert_common_user(data, cur)
        patient_id= insert_patient(data,cur)
        conn.commit()
        return jsonify({'status': 200, 'results': user_id}), 200
    except Exception as e:
        conn.rollback()
        return jsonify({'status': 500, 'errors': str(e)}), 500
    finally:
        cur.close()
        conn.close()

def insert_doctor(data, employee_id, cur):
    query = '''INSERT INTO doctor (medlicense, employee_users_user_id) 
               VALUES (%s, %s) RETURNING employee_users_user_id'''
    cur.execute(query, (data['medlicense'], employee_id))
    return cur.fetchone()[0]

@app.route('/Projeto/register/doctor', methods=['POST'])
def register_doctor():
    data = request.get_json()
    conn = db_connection()  
    cur = conn.cursor()
    try:
        user_id = insert_common_user(data, cur)
        employee_id = insert_employee(data, user_id, cur)
        doctor_id = insert_doctor(data, employee_id, cur)
        conn.commit()
        return jsonify({'status': 200, 'results': doctor_id}), 200
    except Exception as e:
        conn.rollback()
        return jsonify({'status': 500, 'errors': str(e)}), 500
    finally:
        cur.close()
        conn.close()

def insert_nurse(data, employee_id, cur):
    query = '''INSERT INTO nurse (internal_category, employee_users_user_id) 
               VALUES (%s, %s) RETURNING employee_users_user_id'''
    cur.execute(query, (data['internal_category'], employee_id))
    return cur.fetchone()[0]

@app.route('/Projeto/register/nurse', methods=['POST'])
def register_nurse():
    data = request.get_json()
    conn = db_connection() 
    cur = conn.cursor()
    try:
        user_id = insert_common_user(data, cur)
        employee_id = insert_employee(data, user_id, cur)
        nurse_id = insert_nurse(data, employee_id, cur)
        conn.commit()
        return jsonify({'status': 200, 'results': nurse_id}), 200
    except Exception as e:
        conn.rollback()
        return jsonify({'status': 500, 'errors': str(e)}), 500
    finally:
        cur.close()
        conn.close()

def insert_assistant(data, employee_id, cur):
    query = '''INSERT INTO assistant (area, employee_users_user_id) 
               VALUES (%s, %s) RETURNING employee_users_user_id'''
    cur.execute(query, (data['area'], employee_id))
    return cur.fetchone()[0]

@app.route('/Projeto/register/assistant', methods=['POST'])
def register_assistant():
    data = request.get_json()
    conn = db_connection()  
    cur = conn.cursor()
    try:
        user_id = insert_common_user(data, cur)
        employee_id = insert_employee(data, user_id, cur)
        assistant_id = insert_assistant(data, employee_id, cur)
        conn.commit()
        return jsonify({'status': 200, 'results': assistant_id}), 200
    except Exception as e:
        conn.rollback()
        return jsonify({'status': 500, 'errors': str(e)}), 500
    finally:
        cur.close()
        conn.close()

@app.route('/Projeto/user', methods=['PUT'])
def authenticate_user():
    data = request.get_json()
    
    # Verifica se o nome de usuário e a senha foram fornecidos
    if 'username' not in data or 'password' not in data:
        return jsonify({'status': 400, 'errors': 'Username and password are required'}), 400
    
    conn = db_connection()
    cur = conn.cursor()

    try:
        # Consulta para encontrar o usuário pelo nome e senha
        query = '''SELECT user_id FROM users WHERE username = %s AND password = %s'''
        cur.execute(query, (data['username'], data['password']))
        user = cur.fetchone()

        if user:
            token = generate_token(user[0])
            return jsonify({'status': 200, 'results': token}), 200
        else:
            return jsonify({'status': 400, 'errors': 'Invalid credentials'}), 400
    except Exception as e:
        print(f"Ocorreu uma exceção: {e}")
        return jsonify({'status': 500, 'errors': 'Internal server error'}), 500
    finally:
        cur.close()
        conn.close()


@app.route('/Projeto/appointment', methods=['POST'])
def schedule_appointment():
    data = request.get_json()
    token = request.headers.get('Authorization')

    if not token:
        return jsonify({'status': 400, 'errors': 'Authorization token is missing'}), 400

    try:
        decoded_token = jwt.decode(token, app.config['SECRET_KEY'], algorithms=['HS256'])
        user_id = decoded_token['user_id']
    except jwt.ExpiredSignatureError:
        return jsonify({'status': 400, 'errors': 'Token has expired'}), 400
    except jwt.InvalidTokenError:
        return jsonify({'status': 400, 'errors': 'Invalid token'}), 400

    conn = db_connection()
    cur = conn.cursor()

    try:
        query = '''SELECT users_user_id FROM patient WHERE users_user_id = %s'''
        cur.execute(query, (user_id,))
        user = cur.fetchone()
        
        if not user:
            return jsonify({'status': 403, 'errors': 'Only patients can schedule appointments'}), 403

        if 'date' not in data or 'doctor_id' not in data:
            return jsonify({'status': 400, 'errors': 'Missing required fields'}), 400

        query_event = '''
            INSERT INTO event (envent_id, event_date, patient_users_user_id) 
            VALUES (%s, %s, %s) RETURNING envent_id
        '''
        cur.execute(query_event, (data['envent_id'], data['date'], user_id))
        event_id = cur.fetchone()[0]


        query_appointment = '''
            INSERT INTO appointement (event_envent_id, assistant_employee_users_user_id, doctor_employee_users_user_id) 
            VALUES (%s, %s, %s) RETURNING event_envent_id, bill_billid
        '''
        cur.execute(query_appointment, (event_id,data['assistant_employee_users_user_id'], data['doctor_id']))
        result = cur.fetchone()

        if result:  # Verifica se a consulta retornou algum resultado
            appointment_id = result[0]
            bill_billid=result[1]
        else:
            raise Exception("Failed to retrieve appointment_id")
        
        query_update_bill = '''
            UPDATE bill
            SET totalcost = totalcost + %s, amounttopay = amounttopay
            WHERE billid = %s
        '''
        cur.execute(query_update_bill, (data['amounttopay'], bill_billid))

        conn.commit()
        return jsonify({'status': 200, 'results': appointment_id}), 200
    except Exception as e:
        conn.rollback()
        print(f"Exception occurred: {e}")
        return jsonify({'status': 500, 'errors': 'Internal server error'}), 500
    finally:
        cur.close()
        conn.close()

@app.route('/Projeto/appointments/<int:patient_user_id>', methods=['GET'])
def see_appointments(patient_user_id):
    token = request.headers.get('Authorization')

    if not token:
        return jsonify({'status': 400, 'errors': 'Authorization token is missing'}), 400

    try:
        decoded_token = jwt.decode(token, app.config['SECRET_KEY'], algorithms=['HS256'])
        user_id = decoded_token['user_id']
    except jwt.ExpiredSignatureError:
        return jsonify({'status': 400, 'errors': 'Token has expired'}), 400
    except jwt.InvalidTokenError:
        return jsonify({'status': 400, 'errors': 'Invalid token'}), 400

    conn = db_connection()
    cur = conn.cursor()

    try:
        query_assistant = '''SELECT * FROM assistant WHERE employee_users_user_id = %s'''
        cur.execute(query_assistant, (user_id,))
        is_assistant = cur.fetchone()

        query_patient = '''SELECT * FROM patient WHERE users_user_id = %s'''
        cur.execute(query_patient, (user_id,))
        is_patient = cur.fetchone()

        if not is_assistant and not is_patient and user_id != patient_user_id:
            return jsonify({'status': 403, 'errors': 'Access denied'}), 403

        query = '''
            SELECT 
                e.*,
                a.*
            FROM 
                event e
            JOIN 
                appointement a ON e.envent_id = a.event_envent_id
            WHERE 
                e.patient_users_user_id = %s
        '''
        cur.execute(query, (patient_user_id,))
        appointments = cur.fetchall()
        results = []
        for appointment in appointments:
            results.append({
                'event_id': appointment[0],
                'event_date': appointment[1],
                'patient_id': appointment[2],
                'assistant_id': appointment[3],
                'bill_id': appointment[4],
                'doctor_id': appointment[5],
                'appointment_id': appointment[6]
            })

        return jsonify({'status': 200, 'results': results}), 200
    except Exception as e:
        return jsonify({'status': 500, 'errors': str(e)}), 500
    finally:
        cur.close()
        conn.close()

def gerar_hospitalization_id(cur):
    while True:
        # Gera um novo ID único para a hospitalização
        new_hospitalization_id = random.randint(1, 101)
        query_check_id = '''SELECT COUNT(*) FROM hospitalization WHERE event_envent_id = %s'''
        cur.execute(query_check_id, (new_hospitalization_id,))
        count = cur.fetchone()[0]
        if count == 0:
            return new_hospitalization_id

@app.route('/Projeto/surgery', methods=['POST'])
@app.route('/Projeto/surgery/<int:hospitalization_id>', methods=['POST'])
def schedule_surgery(hospitalization_id=None):
    data = request.get_json()
    token = request.headers.get('Authorization')

    if not token:
        return jsonify({'status': 400, 'errors': 'Authorization token is missing'}), 400

    try:
        decoded_token = jwt.decode(token, app.config['SECRET_KEY'], algorithms=['HS256'])
        user_id = decoded_token['user_id']
    except jwt.ExpiredSignatureError:
        return jsonify({'status': 400, 'errors': 'Token has expired'}), 400
    except jwt.InvalidTokenError:
        return jsonify({'status': 400, 'errors': 'Invalid token'}), 400

    conn = db_connection()
    cur = conn.cursor()

    try:
        # Verifica se o usuário é um assistente
        query_assistant = '''SELECT * FROM assistant WHERE employee_users_user_id = %s'''
        cur.execute(query_assistant, (user_id,))
        is_assistant = cur.fetchone()

        if not is_assistant:
            return jsonify({'status': 403, 'errors': 'Access denied'}), 403

        query_event = '''
            INSERT INTO event (envent_id, event_date, patient_users_user_id) 
            VALUES (%s, %s, %s) RETURNING envent_id
        '''
        cur.execute(query_event, (data['envent_id'], data['date'], data['patient_id']))
        event_id = cur.fetchone()[0]
        if hospitalization_id is None:
            hospitalization_id = gerar_hospitalization_id(cur)
            query_event2 = '''
                INSERT INTO event (envent_id, event_date, patient_users_user_id) 
                VALUES (%s, %s, %s) RETURNING envent_id
            '''
            cur.execute(query_event2, (hospitalization_id, data['date'], data['patient_id']))
            row = cur.fetchone()
            envent_id = row[0]
            query_hospitalization = '''INSERT INTO hospitalization (entry_date, exit_date, assistant_employee_users_user_id, event_envent_id) VALUES (%s, %s, %s, %s) RETURNING bill_billid'''
            cur.execute(query_hospitalization, (data['date'], data['date_exit'], user_id, envent_id))

        query_get_bill_id = '''
        SELECT bill_billid
        FROM hospitalization
        WHERE event_envent_id = %s
        '''
        cur.execute(query_get_bill_id, (hospitalization_id,))
        row = cur.fetchone()
        if row:
            bill_billid = row[0]
        query_surgery = '''INSERT INTO surgery (event_envent_id, doctor_employee_users_user_id, patient_id, hospitalization_event_envent_id) VALUES (%s, %s, %s, %s) RETURNING event_envent_id'''
        cur.execute(query_surgery, (event_id, data['doctor_id'], data['patient_id'], hospitalization_id))
        surgery_id = cur.fetchone()[0]

        for nurse in data['nurses']:
            query_nurse_surgery = '''INSERT INTO nurse_surgery (surgery_event_envent_id, nurse_employee_users_user_id) VALUES (%s, %s)'''
            cur.execute(query_nurse_surgery, (surgery_id, nurse))
        
        query_update_bill = '''
            UPDATE bill
            SET totalcost = totalcost + %s, amounttopay = amounttopay
            WHERE billid = %s
        '''
        cur.execute(query_update_bill, (data['amounttopay'], bill_billid))


        conn.commit()
        return jsonify({'status': 200, 'results': {
            'hospitalization_id': hospitalization_id,
            'surgery_id': surgery_id,
            'patient_id': data['patient_id'],
            'doctor_id': data['doctor_id'],
            'date': data['date']
        }}), 200
    except Exception as e:
        conn.rollback()
        return jsonify({'status': 500, 'errors': str(e)}), 500
    finally:
        cur.close()
        conn.close()


@app.route('/Projeto/prescriptions/<int:person_id>', methods=['GET'])
def get_prescriptions(person_id):
    token = request.headers.get('Authorization')

    if not token:
        return jsonify({'status': 400, 'errors': 'Authorization token is missing'}), 400

    try:
        decoded_token = jwt.decode(token, app.config['SECRET_KEY'], algorithms=['HS256'])
        user_id = decoded_token['user_id']
    except jwt.ExpiredSignatureError:
        return jsonify({'status': 400, 'errors': 'Token has expired'}), 400
    except jwt.InvalidTokenError:
        return jsonify({'status': 400, 'errors': 'Invalid token'}), 400

    conn = db_connection()
    cur = conn.cursor()

    try:
        # Verifica se o usuário é o próprio paciente ou um funcionário
        query_patient = '''SELECT * FROM patient WHERE users_user_id = %s'''
        query_employee = '''SELECT * FROM employee WHERE users_user_id = %s'''

        cur.execute(query_patient, (user_id,))
        is_patient = cur.fetchone()

        cur.execute(query_employee, (user_id,))
        is_employee = cur.fetchone()

        if not is_patient and not is_employee:
            return jsonify({'status': 403, 'errors': 'Access denied'}), 403

        if not is_employee and user_id != person_id:
            return jsonify({'status': 403, 'errors': 'Access denied'}), 403

        # Obter a lista de prescrições para o paciente
        query = '''SELECT p.presid, p.validity, m.name, d.quantity, f.freq
                   FROM prescription p
                   JOIN prescription_medicine pm ON p.presid = pm.prescription_presid
                   JOIN medicine m ON pm.medicine_medicineid = m.medicineid
                   JOIN dosage_medicine dm ON m.medicineid = dm.medicine_medicineid
                   JOIN dosage d ON dm.dosage_quantity = d.quantity
                   JOIN frequency_medicine fm ON m.medicineid = fm.medicine_medicineid
                   JOIN frequency f ON fm.frequency_freq = f.freq
                   WHERE p.presid IN (
                       SELECT prescription_presid
                       FROM appointement_prescription
                       WHERE appointement_event_envent_id IN (
                           SELECT event_envent_id
                           FROM appointement
                           WHERE event_envent_id IN (
                               SELECT envent_id
                               FROM event
                               WHERE patient_users_user_id=%s
                           )
                       )
                       UNION
                       SELECT prescription_presid
                       FROM hospitalization_prescription
                       WHERE hospitalization_event_envent_id IN (
                           SELECT event_envent_id
                           FROM hospitalization
                           WHERE event_envent_id IN (
                               SELECT envent_id
                               FROM event
                               WHERE patient_users_user_id=%s
                           )
                       )
                   )'''
        cur.execute(query, (person_id, person_id))
        prescriptions = cur.fetchall()

        results = {}
        for prescription in prescriptions:
            presid = prescription[0]
            if presid not in results:
                results[presid] = {
                    'prescription_id': presid,
                    'validity': prescription[1],
                    'posology': []
                }
            results[presid]['posology'].append({
                'dose': prescription[3],
                'frequency': prescription[4],
                'medicine': prescription[2]
            })

        return jsonify({'status': 200, 'results': list(results.values())}), 200
    except Exception as e:
        return jsonify({'status': 500, 'errors': str(e)}), 500
    finally:
        cur.close()
        conn.close()

@app.route('/Projeto/prescription', methods=['POST'])
def add_prescription():
    token = request.headers.get('Authorization')

    if not token:
        return jsonify({'status': 400, 'errors': 'Authorization token is missing'}), 400

    try:
        decoded_token = jwt.decode(token, app.config['SECRET_KEY'], algorithms=['HS256'])
        user_id = decoded_token['user_id']
    except jwt.ExpiredSignatureError:
        return jsonify({'status': 400, 'errors': 'Token has expired'}), 400
    except jwt.InvalidTokenError:
        return jsonify({'status': 400, 'errors': 'Invalid token'}), 400

    data = request.get_json()

    if not data:
        return jsonify({'status': 400, 'errors': 'Invalid JSON'}), 400

    event_type = data.get('type')
    presid = data.get('presid')
    event_id = data.get('event_id')
    validity = data.get('validity')
    medicines = data.get('medicines')

    if not event_type or not event_id or not validity or not medicines:
        return jsonify({'status': 400, 'errors': 'Missing required fields'}), 400

    conn = db_connection()
    cur = conn.cursor()

    try:
        # Verifica se o usuário é um médico
        query_doctor = '''SELECT * FROM doctor WHERE employee_users_user_id = %s'''
        cur.execute(query_doctor, (user_id,))
        is_doctor = cur.fetchone()

        if not is_doctor:
            return jsonify({'status': 403, 'errors': 'Access denied'}), 403

        # Inserir a nova prescrição
        insert_prescription = '''INSERT INTO prescription (presid, validity) VALUES (%s, %s) RETURNING presid'''
        cur.execute(insert_prescription, (presid, validity,))
        prescription_id = cur.fetchone()[0]

        # Associar a prescrição ao evento apropriado
        if event_type == 'appointment':
            insert_appointment_prescription = '''INSERT INTO appointement_prescription (appointement_event_envent_id, prescription_presid) VALUES (%s, %s)'''
            cur.execute(insert_appointment_prescription, (event_id, prescription_id))
        elif event_type == 'hospitalization':
            insert_hospitalization_prescription = '''INSERT INTO hospitalization_prescription (hospitalization_event_envent_id, prescription_presid) VALUES (%s, %s)'''
            cur.execute(insert_hospitalization_prescription, (event_id, prescription_id))
        else:
            return jsonify({'status': 400, 'errors': 'Invalid event type'}), 400

        # Inserir os medicamentos e suas dosagens e frequências
        for med in medicines:
            medicine_id = med.get('medicine_id')
            medicine_name = med.get('medicine')
            posology_dose = med.get('posology_dose')
            posology_frequency = med.get('posology_frequency')

            if not medicine_id or not medicine_name or not posology_dose or not posology_frequency:
                return jsonify({'status': 400, 'errors': 'Missing medicine details'}), 400

            # Verificar se o medicamento existe, se não, adicionar
            query_medicine = '''SELECT medicineid FROM medicine WHERE medicineid = %s'''
            cur.execute(query_medicine, (medicine_id,))
            medicine = cur.fetchone()

            if not medicine:
                insert_medicine = '''INSERT INTO medicine (medicineid, name) VALUES (%s, %s) RETURNING medicineid'''
                cur.execute(insert_medicine, (medicine_id, medicine_name,))
                medicine_id = cur.fetchone()[0]

            # Associar o medicamento à prescrição
            insert_prescription_medicine = '''INSERT INTO prescription_medicine (prescription_presid, medicine_medicineid) VALUES (%s, %s)'''
            cur.execute(insert_prescription_medicine, (prescription_id, medicine_id))

            # Inserir dosagem e frequência
            insert_dosage = '''INSERT INTO dosage (quantity) VALUES (%s) ON CONFLICT (quantity) DO NOTHING'''
            cur.execute(insert_dosage, (posology_dose,))

            insert_frequency = '''INSERT INTO frequency (freq) VALUES (%s) ON CONFLICT (freq) DO NOTHING'''
            cur.execute(insert_frequency, (posology_frequency,))

            # Associar dosagem e frequência ao medicamento
            insert_dosage_medicine = '''INSERT INTO dosage_medicine (dosage_quantity, medicine_medicineid) VALUES (%s, %s) ON CONFLICT (dosage_quantity, medicine_medicineid) DO NOTHING'''
            cur.execute(insert_dosage_medicine, (posology_dose, medicine_id))

            insert_frequency_medicine = '''INSERT INTO frequency_medicine (frequency_freq, medicine_medicineid) VALUES (%s, %s) ON CONFLICT (frequency_freq, medicine_medicineid) DO NOTHING'''
            cur.execute(insert_frequency_medicine, (posology_frequency, medicine_id))

        conn.commit()
        return jsonify({'status': 200, 'message': 'Prescription added successfully'}), 201

    except Exception as e:
        conn.rollback()
        return jsonify({'status': 500, 'errors': str(e)}), 500
    finally:
        cur.close()
        conn.close()

@app.route('/Projeto/bills/<int:bill_id>', methods=['POST'])
def execute_payment(bill_id):
    data = request.get_json()
    token = request.headers.get('Authorization')

    if not token:
        return jsonify({'status': 400, 'errors': 'Authorization token is missing'}), 400

    try:
        decoded_token = jwt.decode(token, app.config['SECRET_KEY'], algorithms=['HS256'])
        user_id = decoded_token['user_id']
    except jwt.ExpiredSignatureError:
        return jsonify({'status': 400, 'errors': 'Token has expired'}), 400
    except jwt.InvalidTokenError:
        return jsonify({'status': 400, 'errors': 'Invalid token'}), 400

    conn = db_connection()
    cur = conn.cursor()

    try:
        query = '''
            SELECT e.patient_users_user_id 
            FROM bill b
            JOIN appointement a ON b.billid = a.bill_billid
            JOIN event e ON a.event_envent_id = e.envent_id
            WHERE b.billid = %s
        '''
        cur.execute(query, (bill_id,))
        bill_patient_id = cur.fetchone()

        if not bill_patient_id or bill_patient_id[0] != user_id:
            return jsonify({'status': 403, 'errors': 'Access denied: Only the patient can pay their own bills'}), 403

        query = '''SELECT totalcost, status_paid FROM bill WHERE billid = %s'''
        cur.execute(query, (bill_id,))
        bill_info = cur.fetchone()

        if not bill_info:
            return jsonify({'status': 404, 'errors': 'Bill not found'}), 404

        if bill_info[1]:
            return jsonify({'status': 400, 'errors': 'Bill is already paid'}), 400

        remaining_amount = bill_info[0]
        payment_amount = data.get('amount')

        if payment_amount > remaining_amount:
            return jsonify({'status': 400, 'errors': 'Payment amount exceeds remaining bill amount'}), 400

        new_remaining_amount = remaining_amount - payment_amount
        status_paid = new_remaining_amount == 0

        query = '''
            UPDATE bill 
            SET totalcost=%s, amounttopay = amounttopay+%s, status_paid = %s 
            WHERE billid = %s
        '''
        cur.execute(query, (new_remaining_amount,payment_amount, status_paid, bill_id))

        query = '''
            INSERT INTO paymentmethod (paymentid, amountavaiable, bill_billid) 
            VALUES (%s, %s, %s)
        '''
        cur.execute(query, (data['payment_id'], new_remaining_amount, bill_id))

        conn.commit()
        return jsonify({'status': 200, 'results': new_remaining_amount}), 200
    except Exception as e:
        conn.rollback()
        return jsonify({'status': 500, 'errors': str(e)}), 500
    finally:
        cur.close()
        conn.close()

@app.route('/Projeto/top3', methods=['GET'])
def list_top3_patients():
    token = request.headers.get('Authorization')

    if not token:
        return jsonify({'status': 400, 'errors': 'Authorization token is missing'}), 400

    try:
        decoded_token = jwt.decode(token, app.config['SECRET_KEY'], algorithms=['HS256'])
        user_id = decoded_token['user_id']
    except jwt.ExpiredSignatureError:
        return jsonify({'status': 400, 'errors': 'Token has expired'}), 400
    except jwt.InvalidTokenError:
        return jsonify({'status': 400, 'errors': 'Invalid token'}), 400

    conn = db_connection()
    cur = conn.cursor()

    try:
        # Verifica se o usuário é um assistente
        query_assistant = '''SELECT * FROM assistant WHERE employee_users_user_id = %s'''
        cur.execute(query_assistant, (user_id,))
        is_assistant = cur.fetchone()

        if not is_assistant:
            return jsonify({'status': 403, 'errors': 'Access denied: Only assistants can access this endpoint'}), 403
        query = '''
            SELECT 
                u.username AS patient_name,
                SUM(total_amount_spent) AS total_amount_spent,
                JSON_AGG(
                    JSON_BUILD_OBJECT(
                        'type', event_type,
                        'id', event_id,
                        'doctor_id', doctor_id,
                        'date', event_date
                    )
                ) AS procedures
            FROM 
                (
                    SELECT 
                        p.users_user_id AS patient_id,
                        'appointment' AS event_type,
                        e.envent_id AS event_id,
                        a.doctor_employee_users_user_id AS doctor_id,
                        e.event_date AS event_date,
                        b.amounttopay AS total_amount_spent
                    FROM 
                        patient p
                        JOIN event e ON p.users_user_id = e.patient_users_user_id
                        JOIN appointement a ON e.envent_id = a.event_envent_id
                        JOIN bill b ON a.bill_billid = b.billid
                    WHERE 
                        DATE_TRUNC('month', e.event_date) = DATE_TRUNC('month', CURRENT_DATE)
                    
                    UNION ALL
                    
                    SELECT 
                        p.users_user_id AS patient_id,
                        'hospitalization' AS event_type,
                        h.event_envent_id AS event_id,
                        s.doctor_employee_users_user_id AS doctor_id,
                        h.entry_date AS event_date,
                        b.amounttopay AS total_amount_spent
                    FROM 
                        patient p
                        JOIN event e ON p.users_user_id = e.patient_users_user_id
                        JOIN hospitalization h ON e.envent_id = h.event_envent_id
                        JOIN surgery s ON s.hospitalization_event_envent_id = h.event_envent_id
                        JOIN bill b ON h.bill_billid = b.billid
                    WHERE 
                        DATE_TRUNC('month', e.event_date) = DATE_TRUNC('month', CURRENT_DATE)
                ) AS subquery
                JOIN users u ON subquery.patient_id = u.user_id
            GROUP BY 
                u.username
            ORDER BY 
                SUM(total_amount_spent) DESC
            LIMIT 3
        '''
        cur.execute(query)
        top3 = cur.fetchall()

        results = []
        for row in top3:
            patient_name, amount_spent, procedures = row
            results.append({
                'patient_name': patient_name,
                'amount_spent': float(amount_spent),
                'procedures': procedures
            })

        return jsonify({'status': 200, 'results': results}), 200
    except Exception as e:
        return jsonify({'status': 500, 'errors': str(e)}), 500
    finally:
        cur.close()
        conn.close()

@app.route('/Projeto/daily/<date>', methods=['GET'])
def daily_summary(date):
    token = request.headers.get('Authorization')

    if not token:
        return jsonify({'status': 400, 'errors': 'Authorization token is missing'}), 400

    try:
        decoded_token = jwt.decode(token, app.config['SECRET_KEY'], algorithms=['HS256'])
        user_id = decoded_token['user_id']
    except jwt.ExpiredSignatureError:
        return jsonify({'status': 400, 'errors': 'Token has expired'}), 400
    except jwt.InvalidTokenError:
        return jsonify({'status': 400, 'errors': 'Invalid token'}), 400

    conn = db_connection()
    cur = conn.cursor()

    try:
        # Verifica se o usuário é um assistente
        query_assistant = '''SELECT * FROM assistant WHERE employee_users_user_id = %s'''
        cur.execute(query_assistant, (user_id,))
        is_assistant = cur.fetchone()

        if not is_assistant:
            return jsonify({'status': 403, 'errors': 'Access denied: Only assistants can access this endpoint'}), 403

        # Obtém o valor total gasto em hospitalizações no dia especifico
        query_amount_spent = '''
            SELECT SUM(b.amounttopay) AS total_amount_spent
            FROM hospitalization h
            JOIN bill b ON h.bill_billid = b.billid
            WHERE h.entry_date = %s
        '''
        cur.execute(query_amount_spent, (date,))
        amount_spent_result = cur.fetchone()
        total_amount_spent = float(amount_spent_result[0]) if amount_spent_result[0] else 0

        # Conta o número de cirurgias realizadas no dia especifico
        query_surgeries_count = '''
            SELECT COUNT(*) AS surgeries_count
            FROM surgery s
            JOIN event e ON s.event_envent_id = e.envent_id
            WHERE e.event_date = %s
        '''
        cur.execute(query_surgeries_count, (date,))
        surgeries_count_result = cur.fetchone()
        surgeries_count = surgeries_count_result[0] if surgeries_count_result[0] else 0

        # Conta o número de prescrições no dia especifico
        query_prescriptions_count = '''
            SELECT COUNT(*) AS prescriptions_count
            FROM prescription p
            WHERE p.validity = %s
        '''
        cur.execute(query_prescriptions_count, (date,))
        prescriptions_count_result = cur.fetchone()
        prescriptions_count = prescriptions_count_result[0] if prescriptions_count_result[0] else 0

        return jsonify({'status': 200, 'results': {
            'amount_spent': total_amount_spent,
            'surgeries': surgeries_count,
            'prescriptions': prescriptions_count
        }}), 200
    except Exception as e:
        return jsonify({'status': 500, 'errors': str(e)}), 500
    finally:
        cur.close()
        conn.close()


@app.route('/Projeto/report', methods=['GET'])
def monthly_report():
    token = request.headers.get('Authorization')
    if not token:
        return jsonify({'status': 400, 'errors': 'Authorization token is missing'}), 400

    try:
        decoded_token = jwt.decode(token, app.config['SECRET_KEY'], algorithms=['HS256'])
        user_id = decoded_token['user_id']
    except (jwt.ExpiredSignatureError, jwt.InvalidTokenError) as e:
        return jsonify({'status': 400, 'errors': str(e)}), 400

    conn = db_connection()
    cur = conn.cursor()

    try:
        # Verifica se o usuário é um assistente
        cur.execute("SELECT 1 FROM assistant WHERE employee_users_user_id = %s", (user_id,))
        if not cur.fetchone():
            return jsonify({'status': 403, 'errors': 'Access denied: Only assistants can access this endpoint'}), 403

        # Calcula a data de 12 meses atrás
        twelve_months_ago = date.today() - timedelta(days=365)

        query='''
            SELECT 
                to_char(e.event_date, 'YYYY-MM') AS month_year,
                d.employee_users_user_id AS doctor_id,
                u.username AS doctor_name,
                COUNT(*) AS surgeries_count
            FROM surgery s
            JOIN event e ON s.event_envent_id = e.envent_id
            JOIN doctor d ON s.doctor_employee_users_user_id = d.employee_users_user_id
            JOIN users u ON d.employee_users_user_id = u.user_id
            WHERE e.event_date >= %s
            GROUP BY month_year, doctor_id, doctor_name
            ORDER BY month_year, surgeries_count DESC
        '''
        # Consulta para encontrar os médicos com mais cirurgias nos últimos 12 meses
        cur.execute(query, (twelve_months_ago,))

        result = cur.fetchall()
        surgeries_by_month = {}
        for row in result:
            month_year = row[0]
            if month_year not in surgeries_by_month:
                surgeries_by_month[month_year] = []
            surgeries_by_month[month_year].append({
                'doctor_id': row[1],
                'doctor_name': row[2],
                'surgeries_count': row[3]
            })

        return jsonify({'status': 200, 'results': surgeries_by_month}), 200
    except Exception as e:
        return jsonify({'status': 500, 'errors': str(e)}), 500
    finally:
        cur.close()
        conn.close()

if __name__ == '__main__':
    # set up logging
    logging.basicConfig(filename='log_file.log')
    logger = logging.getLogger('logger')
    logger.setLevel(logging.DEBUG)
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)

    # create formatter
    formatter = logging.Formatter('%(asctime)s [%(levelname)s]:  %(message)s', '%H:%M:%S')
    ch.setFormatter(formatter)
    logger.addHandler(ch)

    host = '127.0.0.1'
    port = 8080
    app.run(host=host, debug=True, threaded=True, port=port)
    logger.info(f'API v1.0 online: http://{host}:{port}')

