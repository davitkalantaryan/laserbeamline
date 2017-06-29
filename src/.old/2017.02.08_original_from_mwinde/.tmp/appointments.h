// appointments.h
// M. Winde, DESY-Zeuthen, 26.06.2002
// 
// allow to wait for the first-in-time appointment of a list

#ifndef APPOINTMENTS_H
#define APPOINTMENTS_H

#include <list>
#include <functional>

#include "timeClass.h"
using namespace std;


class appointmentList;
class appointment;
//struct isLater;

typedef appointment* appPointer;

/// a point in time (timePoint) that is allocated to a list of appointments (appointmentList)
/**	It is sometimes usefull to let a thread wait a certain time. This can easyly be done
	by a call timeInterval(3.0).wait(); e. g.
	On the other hand sometimes a thread has to wait until the first of a number of events in time will be reached.
	This can be done by waiting for a an appointmentList. An appointment is an entry to such a list.
	Example:
\code
appointmentList cyclicAppointments;

appointment iAmAliveAppointment = new appointment(&cyclicAppointments, timeInterval(3.0));
appointment showAppointment = new appointment(&cyclicAppointments, timeInterval(4.0));
...

for(;;) {
	cyclicAppointments.wait();
	// returns here after the first appointment in the list was reached
	if(iAmAliveAppointment->reached() {
		... // react
		iAmAliveAppointment->make(timeInterval(3.0));	// make next appointment
	}
	if(showAppointment->reached() {
		...
	}
}
\endcode
*/
class appointment
{
friend class appointmentList;
friend struct isLater;

public:
	// creators
	appointment(appointmentList* theList);	///< create an inactive (empty) appointment
											/**< \param theList pointer to the list of appointments it belongs to */
	appointment(appointmentList* theList, const timeInterval& ti);	///< create an active appointment after an interval in time
																	/**< \param theList pointer to the list of appointments it belongs to 
																		 \param ti the timeInterval (from now), after which the appointment will be reached*/
	appointment(appointmentList* theList, const timePoint& tp);		///< create an active appointment at a point in time
																	/**< \param theList pointer to the list of appointments it belongs to 
																		 \param tp the timePoint, at which the appointment will be reached*/
	
	// destructor
	~appointment();
	
	// members
	void make(const timeInterval& ti);	///< set the time of an inactive appointment, make the appointment activ; or change the time of an active  appointment
										/**< \param  ti the timeInterval (from now), after which the appointment will be reached*/
	void make(const timePoint& tp);		///< set the time of an inactive appointment, make the appointment activ; or change the time of an active  appointment
										/**< \param  tp the timePoint, at which the appointment will be reached*/
	
	bool reached();		///< check whether the appointment is reached
						/**< \return appointment is active and time of appointment is reached */
						
	void done();		///< mark the appointment as inactive
						
	bool isActive() {return active;}		///< check whether the appointment is active
											/**< \return appointment is active  */
	const timePoint* getTimePoint() {return &itsTimePoint;}		/// get the time of an appointment
																/**< the appointment may be active or inactive, the timePoint may be reached or still in future
																	\return the timePoint of the appointment
																	 */
	
						
private:

	bool active;
	timePoint itsTimePoint;
	appointmentList* itsList;
	
};

/// a list of pointers to appointments arranged by their times
class appointmentList : public list<appPointer>
{
friend class appointment;

public:
	
	/// wait for the first (in time) appointment of the list
	const appPointer wait();		/**< do nothing (pause the thread) until the next appointment
									 \return pointer to the appointment that was reached */
	
private:

	void insertBasedOnTime(appPointer anApp);
	
	struct isLater : public binary_function<appPointer, appPointer, bool> {
		bool operator() (const appPointer& x, const appPointer& y) const {
			return (!(x->isActive())) ||  (*(x->getTimePoint()) > *(y->getTimePoint()));
		}
	}; /**< used internally only */
};


#endif
