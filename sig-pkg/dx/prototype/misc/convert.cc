/*******************************************************************************

 File:    convert.cc
 Project: OpenSonATA
 Authors: The OpenSonATA code is the result of many programmers
          over many years

 Copyright 2011 The SETI Institute

 OpenSonATA is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 OpenSonATA is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
 
 Implementers of this code are requested to include the caption
 "Licensed through SETI" with a link to setiQuest.org.
 
 For alternate licensing arrangements, please contact
 The SETI Institute at www.seti.org or setiquest.org. 

*******************************************************************************/

/**********************************************************************
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/convert.cc,v $
**
** Network functions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/convert.cc,v 1.1.1.1 2006/08/02 05:30:28 kes Exp $
** $Log: convert.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:28  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:53  kes
** *** empty log message ***
**
 * Revision 1.4  98/10/29  11:42:57  kes
 * NSS version
 * 
 * Revision 1.3  96/05/15  18:26:45  kes
 * Corrected bug in ConvertTuneRequest.
 * 
 * Revision 1.2  96/01/31  11:40:02  kes
 * Added function prototypes, changed header files, changed
 * ConvertErrorMessage to ConvertErrorMsg, added ntohd().
 * 
 * Revision 1.1  96/01/24  17:49:40  kes
 * Initial revision
 * 
 * Revision 1.1  96/01/24  13:01:01  kes
 * Initial revision
 * 
 * Revision 1.1  96/01/22  17:35:39  kes
 * Initial revision
 * 
**********************************************************************/

#include <sys/types.h>
#include <netinet/in.h>
#include "nss.h"
#include "nssmsg.h"
#include "convert.h"

/* prototypes */
void ConvertConfigStatus(config_status_t *);
void ConvertFuddSignif(fudd_signif_t *);
void ConvertFuddChiSq(fudd_chisq_t *);
void ConvertFuddChiSqSignif(fudd_chisq_signif_t *);
void ConvertGenConfig(gen_config_t *);
void ConvertCWDConfig(cwd_config_t *);
void ConvertPDConfig(pd_config_t *);
void ConvertFuddConfig(fudd_config_t *);
void ConvertCandidateDescription(candidate_description_t *);
void ConvertCandidateReport(candidate_report_t *);
void ConvertFuddCandidates(fudd_candidates_t *);
void ConvertSignalData(signal_data_t *);
void ConvertCWDSegment(cwd_segment_t *);
void ConvertCWDSignal(cwd_signal_t *);
void ConvertCWDSignalReport(cwd_signal_report_t *);
void ConvertPDSignalReport(pd_signal_report_t *);
void ConvertFuddSignalReport(fudd_signal_report_t *);
void ConvertFuddReport(fudd_report_t *);
void ConvertFuddResultsSummary(fudd_results_summary_t *);
void ConvertTuneRequest(if_tune_request_t *);
void ConvertConfiguration(configuration_t *);
void ConvertETC(long *);
void ConvertErrorMsg(tss_error_t *);
void ConvertSETITime(setitime_t *);
void ConvertBCTMTime(seti_bc_tm_t *);

double ntohd(volatile double);
#define htond(double)		ntohd(double)

void
ConvertMsgHeader(msg_header_t *header)
{
	header->code = ntohl(header->code);
	header->sender = ntohs(header->sender);
	header->receiver = ntohs(header->receiver);
	header->length = ntohl(header->length);
	header->activity_id = ntohl(header->activity_id);
	header->request_id = ntohl(header->request_id);
	ConvertSETITime(&header->time_sent);
	ConvertSETITime(&header->time_recv);
	ConvertSETITime(&header->time_proc);
}

error_t
ConvertMsgData(fudd_packet_t *p)
{
	switch (p->header.code) {
	case FUDD_CONFIG_STATUS_REPORT:
		ConvertConfigStatus((config_status_t *) p->data);
		break;
	case FUDD_SITE_CONFIG_PARAMETERS:
		ConvertConfiguration((configuration_t *) p->data);
		break;
	case FUDD_CONFIGURE:
		ConvertFuddConfig((fudd_config_t *) p->data);
		break;
	case FUDD_SIGNIFICANCE:
		ConvertFuddChiSqSignif((fudd_chisq_signif_t *) p->data);
		break;
	case FUDD_ERROR_MESSAGE:
		ConvertErrorMsg((tss_error_t *) p->data);
		break;
	case FUDD_TUNE_IF_REQUEST:
	case FUDD_IF_READY:
		ConvertTuneRequest((if_tune_request_t *) p->data);
		break;
	case FUDD_CANDIDATE_LIST:
		ConvertFuddCandidates((fudd_candidates_t *) p->data);
		break;
	case FUDD_ESTIMATE_TO_COMPLETE:
		ConvertETC((long *) p->data);
		break;
	case FUDD_CANDIDATE_RESULTS:
		ConvertFuddResultsSummary((fudd_results_summary_t *) p->data);
		break;
	default:
		break;
	}
}

void
ConvertConfigStatus(config_status_t *c)
{
	int i;

	c->state = ntohl(c->state);
	c->activity_id = ntohl(c->activity_id);
	ConvertBCTMTime(&c->current_time);
	c->type_of_last_test = ntohl(c->type_of_last_test);
	ConvertSETITime(&c->time_of_last_test);
	ConvertSETITime(&c->startup_time);
	for (i = 0; i < 9; i++)
		c->conn_status[i] = ntohl(c->conn_status[i]);
}

void
ConvertFuddChiSqSignif(fudd_chisq_signif_t *c)
{
	int i, j, k;

	ConvertFuddSignif(&c->dadd_signif);
	ConvertFuddSignif(&c->coherent_signif);
	for (i = 0; i < MAX_OBS_LENGTHS; i++)
		ConvertFuddChiSq(&c->dadd[i]);
	for (i = 0; i < MAX_DSEARCH; i++) {
		for (j = 0; j < MAX_FSEARCH; j++) {
			for (k = 0; k < MAX_WSEARCH; k++) {
				ConvertFuddChiSq(&c->coherent_exact[i][j][k]);
				ConvertFuddChiSq(&c->coherent_range[i][j][k]);
			}
		}
	}
}

void
ConvertFuddSignif(fudd_signif_t *s)
{
	int i;

	for (i = 0; i < MAX_SIGNIF; i++)
		s->signif[i] = ntohd(s->signif[i]);
}

void
ConvertFuddChiSq(fudd_chisq_t *c)
{
	int i;

	for (i = 0; i < MAX_SIGNIF; i++)
		c->chisq[i] = ntohd(c->chisq[i]);
}

void
ConvertGenConfig(gen_config_t *c)
{
#ifdef notdef
	c->detection_mode = ntohl(c->detection_mode);
	c->dummy = ntohl(c->dummy);
#endif
	c->min_avg_bin_power = ntohd(c->min_avg_bin_power);
	c->max_avg_bin_power = ntohd(c->max_avg_bin_power);
}

void
ConvertCWDConfig(cwd_config_t *c)
{
	c->dadd_threshold = ntohd(c->dadd_threshold);
	c->main_coherent_threshold = ntohd(c->main_coherent_threshold);
	c->remote_coherent_threshold = ntohd(c->remote_coherent_threshold);
	c->freq_search = ntohl(c->freq_search);
	c->drift_search = ntohl(c->drift_search);
	c->width_search = ntohl(c->width_search);
	c->dummy = ntohl(c->dummy);
}

void ConvertPDConfig(pd_config_t *c)
{
	c->pulse_threshold = ntohd(c->pulse_threshold);
	c->huge_pulse_threshold = ntohd(c->huge_pulse_threshold);
	c->main_threshold = ntohd(c->main_threshold);
	c->remote_threshold = ntohd(c->remote_threshold);
}

void
ConvertFuddConfig(fudd_config_t *c)
{
	ConvertGenConfig(&c->fudd);
	ConvertCWDConfig(&c->cwd);
	ConvertPDConfig(&c->pd);
}

void
ConvertCandidateDescription(candidate_description_t *c)
{
	c->candidate_id = ntohl(c->candidate_id);
	c->signal_type = ntohl(c->signal_type);
	c->polarization = ntohl(c->polarization);
	c->resolution = ntohl(c->resolution);
	c->number_of_pulses = ntohl(c->number_of_pulses);
	c->rf_freq = ntohd(c->rf_freq);
	c->drift = ntohd(c->drift);
	c->power = ntohd(c->power);
	c->signal_width = ntohd(c->signal_width);
	c->pulse_period = ntohd(c->pulse_period);
}

void
ConvertCandidateReport(candidate_report_t *c)
{
	int i;

	c->candidate_count = ntohl(c->candidate_count);
	c->dummy = ntohl(c->dummy);
	c->center_frequency = ntohd(c->center_frequency);
	for (i = 0; i < MAX_REOBS_CANDIDATES; i++)
		ConvertCandidateDescription(&c->list[i]);
}

void
ConvertFuddCandidates(fudd_candidates_t *c)
{
	int i;

	c->f_ratio = ntohd(c->f_ratio);
	c->drift_offset_factor = ntohd(c->drift_offset_factor);
	c->curv_main_factor = ntohd(c->curv_main_factor);
	c->curv_remote_factor = ntohd(c->curv_remote_factor);

	c->archive_type = ntohl(c->archive_type);
	c->obs_id = ntohl(c->obs_id);
	c->seti_starnum = ntohl(c->seti_starnum);
	ConvertBCTMTime(&c->fudd_start_time);
	c->fudd_obs_length = ntohl(c->fudd_obs_length);
	ConvertBCTMTime(&c->tss_start_time);
	c->tss_obs_length = ntohl(c->tss_obs_length);

	for (i = 0; i < MAX_TSS; i++)
		ConvertCandidateReport(&c->tss[i]);
}

void
ConvertSignalData(signal_data_t *s)
{
	s->site = ntohl(s->site);
	s->type = ntohl(s->type);
#ifdef notdef
	s->bin_width = ntohd(s->bin_width);
#endif
	s->rf_freq = ntohd(s->rf_freq);
	s->drift = ntohd(s->drift);
	s->power = ntohd(s->power);
	s->width = ntohd(s->width);
	s->chisq = ntohd(s->chisq);
	s->signif = ntohd(s->signif);
	s->snr = ntohd(s->snr);
}

void
ConvertCWDSegment(cwd_segment_t *s)
{
	s->segment = ntohl(s->segment);
	s->dummy = ntohl(s->dummy);
	ConvertSignalData(&s->signal);
}

void
ConvertCWDSignal(cwd_signal_t *s)
{
	int i;

	s->nsegments = ntohl(s->nsegments);
	s->nreports = ntohl(s->nreports);
	ConvertSignalData(&s->dadd);
	ConvertSignalData(&s->main);
	ConvertSignalData(&s->remote);

	for (i = 0; i < MAX_FUDD_SEGMENTS * MAX_FUDDS; i++)
		ConvertCWDSegment(&s->segment[i]);
}

void
ConvertCWDSignalReport(cwd_signal_report_t *r)
{
	ConvertCWDSignal(&r->left);
	ConvertCWDSignal(&r->right);
}

void
ConvertPDSignalReport(pd_signal_report_t *s)
{
	s->number_of_pulses = ntohl(s->number_of_pulses);
	s->dummy = ntohl(s->dummy);
	s->pulse_period = ntohd(s->pulse_period);
	ConvertSignalData(&s->main);
	ConvertSignalData(&s->remote);
}

void
ConvertFuddSignalReport(fudd_signal_report_t *r)
{
	int i, n;

	r->candidate_id = ntohl(r->candidate_id);
	r->signal_type = ntohl(r->signal_type);
	r->polarization = ntohl(r->polarization);
	r->nsites = ntohl(r->nsites);
	if (r->signal_type == SDS_PD || r->signal_type == ntohl(SDS_PD))
		ConvertPDSignalReport(&r->signal.pd);
	else
		ConvertCWDSignalReport(&r->signal.cwd);
}

void
ConvertFuddReport(fudd_report_t *r)
{
	int i;

	r->candidate_count = ntohl(r->candidate_count);
	r->obs_id = ntohl(r->obs_id);
	r->seti_starnum = ntohl(r->seti_starnum);
	r->obs_length = ntohl(r->obs_length);
	r->center_frequency = ntohd(r->center_frequency);
	for (i = 0; i < MAX_REOBS_CANDIDATES; i++)
		ConvertFuddSignalReport(&r->list[i]);
}	

void
ConvertFuddResultsSummary(fudd_results_summary_t *s)
{
	int i;

	for (i = 0; i < MAX_TSS; i++)
		ConvertFuddReport(&s->tss[i]);
}

void
ConvertTuneRequest(if_tune_request_t *t)
{
	int i;

	for (i = 0; i < MAX_FUDD_PROCESSORS; i++) {
		t[i].site = ntohl(t[i].site);
		t[i].type = ntohl(t[i].type);
		t[i].qbbc = ntohl(t[i].qbbc);
		t[i].dummy = ntohl(t[i].dummy);
		t[i].frequency = ntohd(t[i].frequency);
	}
}

void
ConvertConfiguration(configuration_t *c)
{
	c->main_site = ntohl(c->main_site);
	c->remote_site = ntohl(c->remote_site);
	c->SNR_ratio = ntohd(c->SNR_ratio);
	c->main_false_alarm_rate = ntohd(c->main_false_alarm_rate);
	c->remote_false_alarm_rate = ntohd(c->remote_false_alarm_rate);
	c->a2d_clockrate = ntohd(c->a2d_clockrate);
}

void
ConvertETC(long *t)
{
	*t = ntohl(*t);
}

void
ConvertErrorMsg(tss_error_t *t)
{
	t->code = ntohs(t->code);
	t->severity = ntohs(t->severity);
}

void
ConvertSETITime(setitime_t *t)
{
	t->tv_sec = ntohl(t->tv_sec);
	t->tv_usec = ntohl(t->tv_usec);
}

void
ConvertBCTMTime(seti_bc_tm_t *t)
{
	t->tm_nsec = ntohl(t->tm_nsec);
	t->tm_sec = ntohl(t->tm_sec);
	t->tm_min = ntohl(t->tm_min);
	t->tm_hour = ntohl(t->tm_hour);
	t->tm_yday = ntohl(t->tm_yday);
}

/*
** ntohd: convert network byte-ordering to host format
**
** Synopsis:
**		oval = ntohd(ival);
**		double oval;			output value
**		double ival;			input value
** Returns:
**		Byte-swapped version of input value.
** Description:
**		Swaps the order of the bytes in the double.
** Notes:
**		ntohd and htond are the same function.
*/
double
ntohd(volatile double x)
{
	register char *p1, *p2, tmp;

	p1 = (char *) &x;
	p2 = p1 + sizeof(double) - 1;
	do {
		tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
	} while (++p1 < --p2);
	return (x);
}