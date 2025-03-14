----------------------------------------------------------
--            Henrique Oliveira 2022211169              --
--            Andre Silva 2020228054                    --
--            Denzel Bila 2021244709                    --
----------------------------------------------------------
CREATE TABLE employee (
	salary	 INTEGER,
	users_user_id INTEGER,
	PRIMARY KEY(users_user_id)
);

CREATE TABLE doctor (
	medlicense		 VARCHAR(512) NOT NULL,
	employee_users_user_id INTEGER,
	PRIMARY KEY(employee_users_user_id)
);

CREATE TABLE assistant (
	area			 VARCHAR(512),
	employee_users_user_id INTEGER,
	PRIMARY KEY(employee_users_user_id)
);

CREATE TABLE surgery (
	patient_id			 INTEGER NOT NULL,
	doctor_employee_users_user_id	 INTEGER,
	hospitalization_event_envent_id INTEGER NOT NULL,
	event_envent_id		 INTEGER,
	PRIMARY KEY(event_envent_id)
);

CREATE TABLE appointement (
	assistant_employee_users_user_id INTEGER NOT NULL,
	bill_billid			 INTEGER NOT NULL,
	doctor_employee_users_user_id	 INTEGER NOT NULL,
	event_envent_id			 INTEGER,
	PRIMARY KEY(event_envent_id)
);

CREATE TABLE hospitalization (
	entry_date			 DATE NOT NULL,
	exit_date			 DATE NOT NULL,
	assistant_employee_users_user_id INTEGER NOT NULL,
	bill_billid			 INTEGER NOT NULL,
	event_envent_id			 INTEGER,
	PRIMARY KEY(event_envent_id)
);

CREATE TABLE prescription (
	presid	 INTEGER,
	validity DATE,
	PRIMARY KEY(presid)
);

CREATE TABLE medicine (
	medicineid INTEGER,
	name	 VARCHAR(512) NOT NULL,
	PRIMARY KEY(medicineid)
);

CREATE TABLE sideeffect (
	description		 VARCHAR(512),
	probability_probability INTEGER NOT NULL,
	severity_severity	 VARCHAR(512) NOT NULL
);

CREATE TABLE bill (
	billid	 INTEGER,
	totalcost	 BIGINT,
	status_paid BOOL,
	amounttopay BIGINT,
	npayments	 INTEGER,
	PRIMARY KEY(billid)
);

CREATE TABLE paymentmethod (
	paymentid	 INTEGER,
	amountavaiable INTEGER,
	bill_billid	 INTEGER NOT NULL,
	PRIMARY KEY(paymentid)
);

CREATE TABLE nurse (
	internal_category	 VARCHAR(512),
	role			 VARCHAR(512),
	employee_users_user_id INTEGER,
	PRIMARY KEY(employee_users_user_id)
);

CREATE TABLE patient (
	users_user_id INTEGER,
	PRIMARY KEY(users_user_id)
);

CREATE TABLE specialization (
	specialization_id INTEGER,
	name		 VARCHAR(512),
	PRIMARY KEY(specialization_id)
);

CREATE TABLE users (
	user_id	 INTEGER,
	username	 VARCHAR(512) NOT NULL,
	password	 VARCHAR(512) NOT NULL,
	email	 VARCHAR(512) NOT NULL,
	conactinfo VARCHAR(512) NOT NULL,
	PRIMARY KEY(user_id)
);

CREATE TABLE dosage (
	quantity INTEGER,
	PRIMARY KEY(quantity)
);

CREATE TABLE event (
	envent_id		 INTEGER,
	event_date		 DATE NOT NULL,
	patient_users_user_id INTEGER NOT NULL,
	PRIMARY KEY(envent_id)
);

CREATE TABLE severity (
	severity VARCHAR(512),
	PRIMARY KEY(severity)
);

CREATE TABLE probability (
	probability INTEGER,
	PRIMARY KEY(probability)
);

CREATE TABLE frequency (
	freq VARCHAR(512),
	PRIMARY KEY(freq)
);

CREATE TABLE frequency_medicine (
	frequency_freq	 VARCHAR(512),
	medicine_medicineid INTEGER,
	PRIMARY KEY(frequency_freq,medicine_medicineid)
);

CREATE TABLE dosage_medicine (
	dosage_quantity	 INTEGER,
	medicine_medicineid INTEGER,
	PRIMARY KEY(dosage_quantity,medicine_medicineid)
);

CREATE TABLE specialization_specialization (
	specialization_specialization_id	 INTEGER,
	specialization_specialization_id1 INTEGER NOT NULL,
	PRIMARY KEY(specialization_specialization_id)
);

CREATE TABLE specialization_doctor (
	specialization_specialization_id INTEGER,
	doctor_employee_users_user_id	 INTEGER,
	PRIMARY KEY(specialization_specialization_id,doctor_employee_users_user_id)
);

CREATE TABLE nurse_surgery (
	nurse_employee_users_user_id INTEGER,
	surgery_event_envent_id	 INTEGER,
	PRIMARY KEY(nurse_employee_users_user_id,surgery_event_envent_id)
);

CREATE TABLE hospitalization_nurse (
	hospitalization_event_envent_id INTEGER,
	nurse_employee_users_user_id	 INTEGER,
	PRIMARY KEY(hospitalization_event_envent_id,nurse_employee_users_user_id)
);

CREATE TABLE nurse_appointement (
	nurse_employee_users_user_id INTEGER,
	appointement_event_envent_id INTEGER,
	PRIMARY KEY(nurse_employee_users_user_id,appointement_event_envent_id)
);

CREATE TABLE medicine_sideeffect (
	medicine_medicineid INTEGER,
	PRIMARY KEY(medicine_medicineid)
);

CREATE TABLE prescription_medicine (
	prescription_presid INTEGER,
	medicine_medicineid INTEGER,
	PRIMARY KEY(prescription_presid,medicine_medicineid)
);

CREATE TABLE appointement_prescription (
	appointement_event_envent_id INTEGER,
	prescription_presid		 INTEGER NOT NULL,
	PRIMARY KEY(appointement_event_envent_id)
);

CREATE TABLE hospitalization_prescription (
	hospitalization_event_envent_id INTEGER,
	prescription_presid		 INTEGER NOT NULL,
	PRIMARY KEY(hospitalization_event_envent_id)
);

ALTER TABLE employee ADD CONSTRAINT employee_fk1 FOREIGN KEY (users_user_id) REFERENCES users(user_id);
ALTER TABLE doctor ADD CONSTRAINT doctor_fk1 FOREIGN KEY (employee_users_user_id) REFERENCES employee(users_user_id);
ALTER TABLE assistant ADD CONSTRAINT assistant_fk1 FOREIGN KEY (employee_users_user_id) REFERENCES employee(users_user_id);
ALTER TABLE surgery ADD CONSTRAINT surgery_fk1 FOREIGN KEY (doctor_employee_users_user_id) REFERENCES doctor(employee_users_user_id);
ALTER TABLE surgery ADD CONSTRAINT surgery_fk2 FOREIGN KEY (hospitalization_event_envent_id) REFERENCES hospitalization(event_envent_id);
ALTER TABLE surgery ADD CONSTRAINT surgery_fk3 FOREIGN KEY (event_envent_id) REFERENCES event(envent_id);
ALTER TABLE appointement ADD CONSTRAINT appointement_fk1 FOREIGN KEY (assistant_employee_users_user_id) REFERENCES assistant(employee_users_user_id);
ALTER TABLE appointement ADD CONSTRAINT appointement_fk2 FOREIGN KEY (bill_billid) REFERENCES bill(billid);
ALTER TABLE appointement ADD CONSTRAINT appointement_fk3 FOREIGN KEY (doctor_employee_users_user_id) REFERENCES doctor(employee_users_user_id);
ALTER TABLE appointement ADD CONSTRAINT appointement_fk4 FOREIGN KEY (event_envent_id) REFERENCES event(envent_id);
ALTER TABLE hospitalization ADD CONSTRAINT hospitalization_fk1 FOREIGN KEY (assistant_employee_users_user_id) REFERENCES assistant(employee_users_user_id);
ALTER TABLE hospitalization ADD CONSTRAINT hospitalization_fk2 FOREIGN KEY (bill_billid) REFERENCES bill(billid);
ALTER TABLE hospitalization ADD CONSTRAINT hospitalization_fk3 FOREIGN KEY (event_envent_id) REFERENCES event(envent_id);
ALTER TABLE sideeffect ADD CONSTRAINT sideeffect_fk1 FOREIGN KEY (probability_probability) REFERENCES probability(probability);
ALTER TABLE sideeffect ADD CONSTRAINT sideeffect_fk2 FOREIGN KEY (severity_severity) REFERENCES severity(severity);
ALTER TABLE paymentmethod ADD CONSTRAINT paymentmethod_fk1 FOREIGN KEY (bill_billid) REFERENCES bill(billid);
ALTER TABLE nurse ADD CONSTRAINT nurse_fk1 FOREIGN KEY (employee_users_user_id) REFERENCES employee(users_user_id);
ALTER TABLE patient ADD CONSTRAINT patient_fk1 FOREIGN KEY (users_user_id) REFERENCES users(user_id);
ALTER TABLE users ADD UNIQUE (username, email, conactinfo);
ALTER TABLE event ADD CONSTRAINT event_fk1 FOREIGN KEY (patient_users_user_id) REFERENCES patient(users_user_id);
ALTER TABLE frequency_medicine ADD CONSTRAINT frequency_medicine_fk1 FOREIGN KEY (frequency_freq) REFERENCES frequency(freq);
ALTER TABLE frequency_medicine ADD CONSTRAINT frequency_medicine_fk2 FOREIGN KEY (medicine_medicineid) REFERENCES medicine(medicineid);
ALTER TABLE dosage_medicine ADD CONSTRAINT dosage_medicine_fk1 FOREIGN KEY (dosage_quantity) REFERENCES dosage(quantity);
ALTER TABLE dosage_medicine ADD CONSTRAINT dosage_medicine_fk2 FOREIGN KEY (medicine_medicineid) REFERENCES medicine(medicineid);
ALTER TABLE specialization_specialization ADD CONSTRAINT specialization_specialization_fk1 FOREIGN KEY (specialization_specialization_id) REFERENCES specialization(specialization_id);
ALTER TABLE specialization_specialization ADD CONSTRAINT specialization_specialization_fk2 FOREIGN KEY (specialization_specialization_id1) REFERENCES specialization(specialization_id);
ALTER TABLE specialization_doctor ADD CONSTRAINT specialization_doctor_fk1 FOREIGN KEY (specialization_specialization_id) REFERENCES specialization(specialization_id);
ALTER TABLE specialization_doctor ADD CONSTRAINT specialization_doctor_fk2 FOREIGN KEY (doctor_employee_users_user_id) REFERENCES doctor(employee_users_user_id);
ALTER TABLE nurse_surgery ADD CONSTRAINT nurse_surgery_fk1 FOREIGN KEY (nurse_employee_users_user_id) REFERENCES nurse(employee_users_user_id);
ALTER TABLE nurse_surgery ADD CONSTRAINT nurse_surgery_fk2 FOREIGN KEY (surgery_event_envent_id) REFERENCES surgery(event_envent_id);
ALTER TABLE hospitalization_nurse ADD CONSTRAINT hospitalization_nurse_fk1 FOREIGN KEY (hospitalization_event_envent_id) REFERENCES hospitalization(event_envent_id);
ALTER TABLE hospitalization_nurse ADD CONSTRAINT hospitalization_nurse_fk2 FOREIGN KEY (nurse_employee_users_user_id) REFERENCES nurse(employee_users_user_id);
ALTER TABLE nurse_appointement ADD CONSTRAINT nurse_appointement_fk1 FOREIGN KEY (nurse_employee_users_user_id) REFERENCES nurse(employee_users_user_id);
ALTER TABLE nurse_appointement ADD CONSTRAINT nurse_appointement_fk2 FOREIGN KEY (appointement_event_envent_id) REFERENCES appointement(event_envent_id);
ALTER TABLE medicine_sideeffect ADD CONSTRAINT medicine_sideeffect_fk1 FOREIGN KEY (medicine_medicineid) REFERENCES medicine(medicineid);
ALTER TABLE prescription_medicine ADD CONSTRAINT prescription_medicine_fk1 FOREIGN KEY (prescription_presid) REFERENCES prescription(presid);
ALTER TABLE prescription_medicine ADD CONSTRAINT prescription_medicine_fk2 FOREIGN KEY (medicine_medicineid) REFERENCES medicine(medicineid);
ALTER TABLE appointement_prescription ADD CONSTRAINT appointement_prescription_fk1 FOREIGN KEY (appointement_event_envent_id) REFERENCES appointement(event_envent_id);
ALTER TABLE appointement_prescription ADD CONSTRAINT appointement_prescription_fk2 FOREIGN KEY (prescription_presid) REFERENCES prescription(presid);
ALTER TABLE hospitalization_prescription ADD CONSTRAINT hospitalization_prescription_fk1 FOREIGN KEY (hospitalization_event_envent_id) REFERENCES hospitalization(event_envent_id);
ALTER TABLE hospitalization_prescription ADD CONSTRAINT hospitalization_prescription_fk2 FOREIGN KEY (prescription_presid) REFERENCES prescription(presid);



---------------------------------------------------------------------------
-- Criação da sequência para gerar valores únicos para billid
CREATE SEQUENCE bill_id_sequence;

-- Criação da função para o trigger a usar no appoitment
CREATE OR REPLACE FUNCTION create_bill_for_appointment()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO bill (billid, totalcost, status_paid, amounttopay, npayments)
    VALUES (NEXTVAL('bill_id_sequence'), 0, FALSE, 0, 1)
    RETURNING billid INTO NEW.bill_billid;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Criação do trigger
CREATE TRIGGER appointment_insert_trigger
BEFORE INSERT ON appointement
FOR EACH ROW EXECUTE FUNCTION create_bill_for_appointment();

----------------------------------------------------------------

-- Criação da função para o trigger a usar na hospitalization
CREATE OR REPLACE FUNCTION create_bill_for_hospitalization()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO bill (billid, totalcost, status_paid, amounttopay, npayments)
    VALUES (NEXTVAL('bill_id_sequence'), 0, FALSE, 0, 1)
    RETURNING billid INTO NEW.bill_billid;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Criação do trigger
CREATE TRIGGER hospitalization_insert_trigger
BEFORE INSERT ON hospitalization
FOR EACH ROW EXECUTE FUNCTION create_bill_for_hospitalization();