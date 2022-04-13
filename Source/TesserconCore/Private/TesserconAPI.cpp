#include "TesserconAPI.h"

static TMap<FString, FString> UsersMap;
static TMap<FString, FTesserconEvent> EventListMap;
static TMap<FString, FTesserconBooth> BoothListMap;

TesserconAPI::TesserconAPI()
{
	HttpModule = &FHttpModule::Get();
	//Users Data
	UsersMap.Add("testattendee@tessercon.com", "testattendeeaccess");
	UsersMap.Add("testexhibitor@tessercon.com", "testexhibitoraccess");
	UsersMap.Add("emptyevent@tessercon.com", "0eventaccess");
	UsersMap.Add("oneevent@tessercon.com", "1eventaccess");
	//Events Data
	FTesserconEvent EventInfo;
	EventInfo.Description = "Tessercon Conference Description";
	EventInfo.EventTitle = "Tessercon Conference";
	EventInfo.EventBannerURL = "https://image.shutterstock.com/image-vector/business-conference-invitation-concept-modern-600w-774390070.jpg";
	EventInfo.EventCompanyID = "www.Tessercon.com";
	EventInfo.EventID = "Tessercon";
	EventInfo.MatrixPublicRoomID = "789456";
	EventInfo.StartTime = FDateTime::UtcNow();
	EventInfo.EndTime = FDateTime::UtcNow();
	EventInfo.VariantName = "PatchVarinat";
	EventInfo.VariantURL = "S3BucketURl";
	EventInfo.UserType = EUserRole::Exhibitor;
	EventListMap.Add("Tessercon", EventInfo);
	//
	EventInfo.Description = "Gsource Conference Description";
	EventInfo.EventTitle = "Gsource Conference";
	EventInfo.EventBannerURL = "https://scontent.fpnq7-2.fna.fbcdn.net/v/t1.6435-9/67933493_2794289487267668_3150933440233209856_n.jpg?stp=cp0_dst-jpg_e15_q65_s320x320&_nc_cat=102&ccb=1-5&_nc_sid=110474&_nc_ohc=D0LiUUYYvjMAX9tN58I&_nc_ht=scontent.fpnq7-2.fna&oh=00_AT8W9BLirDt7BNZ8sHysVii3mku49xzBUBA-APVwI7lyJg&oe=624F3A2A";
	EventInfo.EventCompanyID = "www.gsource.com";
	EventInfo.EventID = "Gsource";
	EventInfo.MatrixPublicRoomID = "789456";
	EventInfo.StartTime = FDateTime::UtcNow();
	EventInfo.EndTime = FDateTime::UtcNow();
	EventInfo.VariantName = "PatchVarinat";
	EventInfo.VariantURL = "S3BucketURl";
	EventInfo.UserType = EUserRole::Attendee;
	EventListMap.Add("Gsource", EventInfo);
	//
	EventInfo.Description = "Intel Conference Description";
	EventInfo.EventTitle = "Intel Conference";
	EventInfo.EventBannerURL = "https://www.techadvisor.com/cmsdata/features/3809839/how_to_watch_intel_alder_lake_event_live_thumb800.jpg";
	EventInfo.EventCompanyID = "www.Intel.com";
	EventInfo.EventID = "Intel";
	EventInfo.MatrixPublicRoomID = "789456";
	EventInfo.StartTime = FDateTime::UtcNow();
	EventInfo.EndTime = FDateTime::UtcNow();
	EventInfo.VariantName = "PatchVarinat";
	EventInfo.VariantURL = "S3BucketURl";
	EventInfo.UserType = EUserRole::Exhibitor;
	EventListMap.Add("Intel", EventInfo);

	//Booths Data
	FTesserconBooth BoothInfo;
	FTesserconUserInfo ExhibitorInfo;
	ExhibitorInfo.FirstName = "Exhibitor1";
	ExhibitorInfo.LastName = "Tessercon";
	ExhibitorInfo.Address = "Pune";
	ExhibitorInfo.CompanyLogoURL = "https://www.gsourcedata.com/assets/images/logo/gsource_logo_lg.png";
	ExhibitorInfo.CompanyName = "Tessercon";
	ExhibitorInfo.Designation = "Developer";
	ExhibitorInfo.Bio = "Exhibitor1 Bio";
	ExhibitorInfo.Phone = "+91 78988 89845";
	ExhibitorInfo.Email = "testattendee@tessercon.com";
	ExhibitorInfo.Website = "www.Tessercon.com";
	ExhibitorInfo.Username = "tess789456123";

	BoothInfo.BoothID = "123";
	BoothInfo.BoothName = "Covid";
	BoothInfo.BoothLocation = "A1";
	BoothInfo.BoothBannerURL = "https://www.printstop.co.in/images/products_gallery_images/Banner1.jpg";
	BoothInfo.BoothDescription = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus scelerisque orci et rhoncus convallis. Duis eu odio eu libero gravida convallis. Aliquam euismod ac ante at sodales. Donec et sodales nisi, eget viverra velit. Praesent mi sapien, lobortis ut egestas ut, gravida vitae nisl. Nulla fringilla, lacus non accumsan pretium, massa urna interdum odio, sed euismod metus nulla tempus sapien. Ut accumsan non massa sed auctor.";
	BoothInfo.CompanyEmail = "tessercon1@gsource.com";
	BoothInfo.CompanyPhone = "+91 98765 43210";
	BoothInfo.BoothExhibitors.Add(ExhibitorInfo);
	BoothListMap.Add("123", BoothInfo);

	ExhibitorInfo.FirstName = "Exhibitor";
	ExhibitorInfo.LastName = "Gsource";
	ExhibitorInfo.Address = "Pune";
	ExhibitorInfo.CompanyLogoURL = "https://www.gsourcedata.com/assets/images/logo/gsource_logo_lg.png";
	ExhibitorInfo.CompanyName = "Gsource";
	ExhibitorInfo.Designation = "Developer";
	ExhibitorInfo.Bio = "Exhibitor2 Bio";
	ExhibitorInfo.Phone = "+91 98567 41230";
	ExhibitorInfo.Email = "testexhibitor@tessercon.com";
	ExhibitorInfo.Website = "www.gsourcedata.com";
	ExhibitorInfo.Username = "tess789456123";

	BoothInfo.BoothID = "178";
	BoothInfo.BoothName = "Photography";
	BoothInfo.BoothLocation = "A2";
	BoothInfo.BoothBannerURL = "https://d1csarkz8obe9u.cloudfront.net/themedlandingpages/tlp_hero_banners-4ee457a41ec5c9a3ff7d870ac465b9bf-1591658664.jpg";
	BoothInfo.BoothDescription = "Lorem ipsum dolor sit amet, consectetur nvallis. Aliquam euismod ac ante at sodales. Donec et sodales nisi, eget viverra veli. gravida vitae nisl. Nulla fringilla, lacus non accumsan pretium, massa urna interdum odio, sed euismod metus nulla tempus sapien. Ut accumsan non massa sed auctor.";
	BoothInfo.BoothExhibitors.Add(ExhibitorInfo);
	BoothInfo.CompanyEmail = "tessercon2@gsource.com";
	BoothInfo.CompanyPhone = "+91 98765 01234";
	BoothListMap.Add("178", BoothInfo);
}

TesserconAPI::~TesserconAPI()
{

}

void TesserconAPI::Login(const FString& email, const FString& Password)
{
	FTesserconLoginResponse LoginResponse;
	if (UsersMap.Find(email) != nullptr)
	{
		if ("pass*123" == Password)
		{
			LoginResponse.Tokens.AccessToken = *UsersMap.Find(email);
			LoginResponse.UserEmail = email;
			LoginResponse.Password = Password;
			
			d_OnLoginResponseReceived.ExecuteIfBound(ETesserconError::Success, LoginResponse, "");
		}
		else
		{
			d_OnLoginResponseReceived.ExecuteIfBound(ETesserconError::Error, LoginResponse, "");
		}
	}
	else
	{
		d_OnLoginResponseReceived.ExecuteIfBound(ETesserconError::EmailError, LoginResponse, "");
	}
}


void TesserconAPI::GetEventsSubscribedListForUser(const FString& AccessToken, const FOnSubscribedEventsListForUserResponseDelegate& TheDelegate)
{
	FTesserconEventsList EventInfo;
	TArray<FTesserconEventsList>EventList;

	if (AccessToken == "0eventaccess")
	{
		TheDelegate.ExecuteIfBound(ETesserconError::Success, EventList);
		return;
	}
	EventInfo.EventID = "Tessercon";
	EventInfo.EventBannerURL = "https://image.shutterstock.com/image-vector/business-conference-invitation-concept-modern-600w-774390070.jpg";
	EventList.Add(EventInfo);
	if (AccessToken == "1eventaccess")
	{
		TheDelegate.ExecuteIfBound(ETesserconError::Success, EventList);
		return;
	}
	EventInfo.EventID = "Gsource";
	EventInfo.EventBannerURL = "https://scontent.fpnq7-2.fna.fbcdn.net/v/t1.6435-9/67933493_2794289487267668_3150933440233209856_n.jpg?stp=cp0_dst-jpg_e15_q65_s320x320&_nc_cat=102&ccb=1-5&_nc_sid=110474&_nc_ohc=D0LiUUYYvjMAX9tN58I&_nc_ht=scontent.fpnq7-2.fna&oh=00_AT8W9BLirDt7BNZ8sHysVii3mku49xzBUBA-APVwI7lyJg&oe=624F3A2A";
	EventList.Add(EventInfo);
	EventInfo.EventID = "Intel";
	EventInfo.EventBannerURL = "https://www.techadvisor.com/cmsdata/features/3809839/how_to_watch_intel_alder_lake_event_live_thumb800.jpg";
	EventList.Add(EventInfo);
	EventInfo.EventID = "HCL";
	EventInfo.EventBannerURL = "https://cdn-az.allevents.in/events5/banners/81d3350bbc022928a6dc16b4ce896a0be9adf4178d60cc73a0aa09ee18f1d83e-rimg-w1200-h600-gmir.jpg?v=1638431072";
	EventList.Add(EventInfo);
	EventInfo.EventID = "Microsoft";
	EventInfo.EventBannerURL = "https://www.microsoft.com/en-us/us-partner-blog/wp-content/uploads/sites/11/2021/08/Cert-Week-Blog.jpg";
	EventList.Add(EventInfo);
	EventInfo.EventID = "QTCon";
	EventInfo.EventBannerURL = "https://i.ytimg.com/vi/l7Rrx74W1fA/maxresdefault.jpg";
	EventList.Add(EventInfo);
	EventInfo.EventID = "Amdocs";
	EventInfo.EventBannerURL = "https://scontent.fpnq7-1.fna.fbcdn.net/v/t1.6435-9/121010587_10157401998006976_6042346633515088052_n.jpg?stp=cp0_dst-jpg_e15_q65_s320x320&_nc_cat=109&ccb=1-5&_nc_sid=2d5d41&_nc_ohc=8qnxqK30DgEAX9YpUv3&_nc_ht=scontent.fpnq7-1.fna&oh=00_AT_HnfibnWtfYmokJeqg42ZNyookgGEhrALj05Bx2nTUXQ&oe=62511F81";
	EventList.Add(EventInfo);
	EventInfo.EventID = "Globant";
	EventInfo.EventBannerURL = "https://i.ytimg.com/vi/GUrcy0QJYOk/maxresdefault.jpg";
	EventList.Add(EventInfo);
	
	if (UsersMap.FindKey(AccessToken) != nullptr)
	{
		TheDelegate.ExecuteIfBound(ETesserconError::Success, EventList);
	}
	else
	{
		EventList.Empty();
		TheDelegate.ExecuteIfBound(ETesserconError::Error, EventList);
	}
}

void TesserconAPI::GetBoothListForEvent(const FString& AccessToken, const FString& EventID, const FOnBoothListForEventResponseDelegate& TheDelegate)
{
	FTesserconBoothList Booth;
	TArray<FTesserconBoothList> BoothList;

	Booth.BoothID = "123";
	Booth.BoothName = "Covid";
	Booth.BoothLocation = "A1";
	Booth.BoothBannerURL = "https://www.printstop.co.in/images/products_gallery_images/Banner1.jpg";
	Booth.BoothMapLocation = FVector(3000.0f, 4150.0f, -490.0f);
	BoothList.Add(Booth);

	Booth.BoothID = "178";
	Booth.BoothName = "Photography";
	Booth.BoothLocation = "A2";
	Booth.BoothBannerURL = "https://d1csarkz8obe9u.cloudfront.net/themedlandingpages/tlp_hero_banners-4ee457a41ec5c9a3ff7d870ac465b9bf-1591658664.jpg";
	Booth.BoothMapLocation = FVector(-3000.0f, 4150.0f, -490.0f);
	BoothList.Add(Booth);

	Booth.BoothID = "132";
	Booth.BoothName = "NewWebSite";
	Booth.BoothLocation = "B1";
	Booth.BoothBannerURL = "https://images.template.net/wp-content/uploads/2016/11/Banner-Feature.jpg";
	Booth.BoothMapLocation = FVector(-3300.0f, -2950.0f, -490.0f);
	BoothList.Add(Booth);

	Booth.BoothID = "1289";
	Booth.BoothName = "Collision";
	Booth.BoothLocation = "B2";
	Booth.BoothBannerURL = "https://mir-s3-cdn-cf.behance.net/projects/404/bf5797136173797.Y3JvcCw1MDMyLDM5MzUsMCww.jpg";
	Booth.BoothMapLocation = FVector(3000.0f, -2950.0f, -490.0f);
	BoothList.Add(Booth);

	if (UsersMap.FindKey(AccessToken) != nullptr)
	{
		TheDelegate.ExecuteIfBound(ETesserconError::Success, BoothList);
	}
	else
	{
		BoothList.Empty();
		TheDelegate.ExecuteIfBound(ETesserconError::Error, BoothList);
	}
}

void TesserconAPI::GetEventInfoForUser(const FString& AccessToken, const FString& EventID, const FOnEventInfoForUserResponseDelegate& TheDelegate)
{
	FTesserconEvent EventInformation;
	if (UsersMap.FindKey(AccessToken) != nullptr)
	{
		if (EventListMap.Find(EventID) != nullptr)
		{
		    EventInformation = *EventListMap.Find(EventID);
			TheDelegate.ExecuteIfBound(ETesserconError::Success, EventInformation);
		}
	}
	else
	{
		TheDelegate.ExecuteIfBound(ETesserconError::Error, EventInformation);	
	}
}

void TesserconAPI::GetBoothInfoForEvent(const FString& AccessToken, const FString& BoothID, const FOnBoothInfoForEventResponseDelegate& TheDelegate)
{
	FTesserconBooth BoothInformation;
	if (UsersMap.FindKey(AccessToken) != nullptr)
	{
		if (BoothListMap.Find(BoothID) != nullptr)
		{
			BoothInformation = *BoothListMap.Find(BoothID);
			TheDelegate.ExecuteIfBound(ETesserconError::Success, BoothInformation);
		}
	}
	else
	{
		TheDelegate.ExecuteIfBound(ETesserconError::Error, BoothInformation);
	}
}

void TesserconAPI::GetOrganiserForEvent(const FString& AccessToken, const FString& EventID, const FOnEventOrganiserResponseDelegate& TheDelegate)
{
	FTesserconEventOrganiser EventOrganiser;
	EventOrganiser.OrganiserName = "Gsource";
	EventOrganiser.OrganiserLogoUrl = "https://media-exp1.licdn.com/dms/image/C4D0BAQFy8y0VKrcIuw/company-logo_200_200/0/1519915041140?e=2147483647&v=beta&t=ebpQPofxr-fho0S3gmFUl_RiGVJn-M6zzG41qnmGqMM";
	EventOrganiser.OrganiserContactUrl = "https://www.gsourcedata.com/";

	FTesserconEventOrganiser EventOrganiserResponse;
	if (UsersMap.FindKey(AccessToken) != nullptr)
	{
		EventOrganiserResponse = EventOrganiser;
		TheDelegate.ExecuteIfBound(ETesserconError::Success, EventOrganiserResponse);
	}
	else
	{
		TheDelegate.ExecuteIfBound(ETesserconError::Error, EventOrganiserResponse);
	}
}

void TesserconAPI::GetSponsorsForEvent(const FString& AccessToken, const FString& EventID, const FOnEventSponsorsResponseDelegate& TheDelegate)
{
	TArray<FString> SponsersList;
	SponsersList.Add("https://png.pngtree.com/element_pic/00/16/07/115783931601b5c.jpg");
	SponsersList.Add("https://png.pngtree.com/element_pic/00/16/09/1557da901899b9d.jpg");
	SponsersList.Add("https://w7.pngwing.com/pngs/828/604/png-transparent-logo-organization-business-service-trade-business-building-company-text.png");

	TArray<FString> EventSponsorsResponse;
	if (UsersMap.FindKey(AccessToken) != nullptr)
	{
		EventSponsorsResponse = SponsersList;
		TheDelegate.ExecuteIfBound(ETesserconError::Success, EventSponsorsResponse);
	}
	else
	{
		TheDelegate.ExecuteIfBound(ETesserconError::Error, EventSponsorsResponse);
	}
}

void TesserconAPI::ForgotPassword(const FString& EmailID, const FOnForgotPasswordForUserResponseDelegate& TheDelegate)
{
	FTesserconLoginResponse LoginResponse;
	
	if (UsersMap.Find(EmailID) != nullptr)
	{
		TheDelegate.ExecuteIfBound(ETesserconError::Success, " ");
	}
	else
	{
		TheDelegate.ExecuteIfBound(ETesserconError::EmailError, " ");
	}
}

void TesserconAPI::GetLoggedinUserInfo(const FString& AccessToken, const FOnLoggedinUserInfoResponseDelegate& TheDelegate)
{
	FTesserconUserInfo UserInfo;
	if (AccessToken == "testattendeeaccess")
	{
		UserInfo.FirstName = "Attendee";
		UserInfo.LastName = "Tessercon";
		UserInfo.Address = "Pune";
		UserInfo.CompanyLogoURL = "https://www.gsourcedata.com/assets/images/logo/gsource_logo_lg.png";
		UserInfo.CompanyName = "Gsource";
		UserInfo.Designation = "Developer";
		UserInfo.Bio = "Nothing New";
		UserInfo.Phone = "+91 89745 61230";
		UserInfo.Email = "testattendee@tessercon.com";
		UserInfo.Website = "www.gsourcedata.com";
		UserInfo.Username = "tess117336637117798955933579942743100164077";
		UserInfo.ProfileImageURL = "https://icon-library.com/images/avatar-icon/avatar-icon-14.jpg";
	}
	if (AccessToken == "testexhibitoraccess")
	{
		UserInfo.FirstName = "Exhibitor";
		UserInfo.LastName = "Tessercon";
		UserInfo.Address = "Pune";
		UserInfo.CompanyLogoURL = "https://www.gsourcedata.com/assets/images/logo/gsource_logo_lg.png";
		UserInfo.CompanyName = "Gsource";
		UserInfo.Designation = "Developer";
		UserInfo.Bio = "Nothing New";
		UserInfo.Phone = "+91 78954 63102";
		UserInfo.Email = "testattendee@tessercon.com";
		UserInfo.Website = "www.gsourcedata.com";
		UserInfo.Username = "tess285684752781164923075739541638533203179";
		UserInfo.ProfileImageURL = "https://cdn3.iconfinder.com/data/icons/business-avatar-1/512/3_avatar-512.png";
	}
	TheDelegate.ExecuteIfBound(ETesserconError::Success, UserInfo);
}


void TesserconAPI::GetUserInfoForUser(const FString& AccessToken, const FString& Username, const FOnUserInfoByUserIDResponseDelegate& TheDelegate)
{
	if (UsersMap.FindKey(AccessToken) != nullptr)
	{
		FTesserconUserInfo UserInfo;
		if ("tess285684752781164923075739541638533203179" == Username)
		{
			//Exhibitor
			UserInfo.FirstName = "Exhibitor";
			UserInfo.LastName = "Tessercon";
			UserInfo.Address = "Pune";
			UserInfo.CompanyLogoURL = "https://cdn3.iconfinder.com/data/icons/business-avatar-1/512/3_avatar-512.png";
			UserInfo.CompanyName = "Gsource";
			UserInfo.Designation = "Developer";
			UserInfo.Bio = "Nothing New";
			UserInfo.Phone = "44445545222";
			UserInfo.Email = "Testattendee@tessercon.com";
			UserInfo.Website = "www.gsourcedata.com";
			UserInfo.Username = "tess285684752781164923075739541638533203179";
			UserInfo.ProfileImageURL = "https://cdn3.iconfinder.com/data/icons/business-avatar-1/512/3_avatar-512.png";
			TheDelegate.ExecuteIfBound(ETesserconError::Success, UserInfo);
		}

		else if ("tess117336637117798955933579942743100164077" == Username)
		{
			//attendee
			UserInfo.FirstName = "Attendee";
			UserInfo.LastName = "Tessercon";
			UserInfo.Address = "Pune";
			UserInfo.CompanyLogoURL = "https://icon-library.com/images/avatar-icon/avatar-icon-14.jpg";
			UserInfo.CompanyName = "Gsource";
			UserInfo.Designation = "Developer";
			UserInfo.Bio = "Nothing New";
			UserInfo.Phone = "44445545222";
			UserInfo.Email = "Testattendee@tessercon.com";
			UserInfo.Website = "www.Tessercon.com";
			UserInfo.Username = "tess117336637117798955933579942743100164077";
			UserInfo.ProfileImageURL = "https://icon-library.com/images/avatar-icon/avatar-icon-14.jpg";
			TheDelegate.ExecuteIfBound(ETesserconError::Success, UserInfo);
		}

		else if ("tess277579840065036435927419150803701660368" == Username)
		{

			UserInfo.FirstName = "Vinit";
			UserInfo.LastName = "Tessercon";
			UserInfo.Address = "Pune";
			UserInfo.CompanyLogoURL = "https://cdn.icon-icons.com/icons2/2643/PNG/512/male_boy_person_people_avatar_icon_159358.png";
			UserInfo.CompanyName = "Gsource";
			UserInfo.Designation = "UI/UX";
			UserInfo.Bio = "Nothing New";
			UserInfo.Phone = "44445545222";
			UserInfo.Email = "Vinit@tessercon.com";
			UserInfo.Website = "www.tessercon.com";
			UserInfo.Username = "tess277579840065036435927419150803701660368";
			UserInfo.ProfileImageURL = "https://cdn.icon-icons.com/icons2/2643/PNG/512/male_boy_person_people_avatar_icon_159358.png";
			TheDelegate.ExecuteIfBound(ETesserconError::Success, UserInfo);
		}

		else if ("tess296884408640382890146014818978608119200" == Username)
		{

			UserInfo.FirstName = "Nagesh";
			UserInfo.LastName = "Tessercon";
			UserInfo.Address = "Pune";
			UserInfo.CompanyLogoURL = "https://freepikpsd.com/file/2019/10/avatar-icon-png-5-Images-PNG-Transparent.png";
			UserInfo.CompanyName = "Gsource";
			UserInfo.Designation = "3D Artist";
			UserInfo.Bio = "Nothing New";
			UserInfo.Phone = "44445545222";
			UserInfo.Email = "Nagesh@tessercon.com";
			UserInfo.Website = "www.tessercon.com";
			UserInfo.Username = "tess296884408640382890146014818978608119200";
			UserInfo.ProfileImageURL = "https://freepikpsd.com/file/2019/10/avatar-icon-png-5-Images-PNG-Transparent.png";
			TheDelegate.ExecuteIfBound(ETesserconError::Success, UserInfo);
		}

		else if ("tess279877786627711864945768279449159047868" == Username)
		{
			UserInfo.FirstName = "Shubham";
			UserInfo.LastName = "Tessercon";
			UserInfo.Address = "Pune";
			UserInfo.CompanyLogoURL = "https://pics.freeicons.io/uploads/icons/png/6822363841598811069-512.png";
			UserInfo.CompanyName = "Gsource";
			UserInfo.Designation = "Developer";
			UserInfo.Bio = "Nothing New";
			UserInfo.Phone = "44445545222";
			UserInfo.Email = "Nagesh@tessercon.com";
			UserInfo.Website = "www.tessercon.com";
			UserInfo.Username = "tess279877786627711864945768279449159047868";
			UserInfo.ProfileImageURL = "https://pics.freeicons.io/uploads/icons/png/6822363841598811069-512.png";
			TheDelegate.ExecuteIfBound(ETesserconError::Success, UserInfo);
		}

		else if ("tess193986455159539452513139833773367684681" == Username)
		{
			UserInfo.FirstName = "sonali";
			UserInfo.LastName = "Tessercon";
			UserInfo.Address = "Pune";
			UserInfo.CompanyLogoURL = "https://cdn.icon-icons.com/icons2/2643/PNG/512/avatar_female_woman_person_people_white_tone_icon_159360.png";
			UserInfo.CompanyName = "Gsource";
			UserInfo.Designation = "Developer";
			UserInfo.Bio = "Nothing New";
			UserInfo.Phone = "44445545222";
			UserInfo.Email = "sonali@tessercon.com";
			UserInfo.Website = "www.tessercon.com";
			UserInfo.Username = "tess193986455159539452513139833773367684681";
			UserInfo.ProfileImageURL = "https://cdn.icon-icons.com/icons2/2643/PNG/512/avatar_female_woman_person_people_white_tone_icon_159360.png";
			TheDelegate.ExecuteIfBound(ETesserconError::Success, UserInfo);
		}
		//@tess296884408640382890146014818978608119200:comms.tessercon.com //nagesh
		//@tess277579840065036435927419150803701660368:comms.tessercon.com //vinit
		//@tess279877786627711864945768279449159047868:comms.tessercon.com //shubham
		//@tess193986455159539452513139833773367684681:comms.tessercon.com //sonali
		else
		{
			TheDelegate.ExecuteIfBound(ETesserconError::Error, UserInfo);
		}
	}
}